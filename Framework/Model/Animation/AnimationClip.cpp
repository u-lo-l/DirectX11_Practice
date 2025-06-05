#include "framework.h"
#include "AnimationClip.h"

AnimationClip::AnimationClip(const CSkeletal * InSkeleton, const wstring& AnimationAssetPath)
	: Skeleton(InSkeleton)
{
	ASSERT(!!InSkeleton, "Skeleton Not Valid")
	vector<KeyFrameData *> KeyFrames;
	ReadAnimationAsset(InSkeleton, AnimationAssetPath, KeyFrames);
	vector<PackedHalfTRS> KeyFramesArray;
	CreateKeyFrameTable(InSkeleton, KeyFrames, KeyFramesArray);
	CreateKeyFrameTexture(KeyFramesArray);
}

AnimationClip::~AnimationClip()
{
	SAFE_DELETE(KeyFrameTexture);
}

float AnimationClip::CalculateNextAnimTime(float CurrentTime, float DeltaSecond) const
{
	const float AnimFullTime = GetAnimationLength() * GetTickPerSecond(); 
	DeltaSecond *= GetTickPerSecond() * GetPlayRate();
	CurrentTime += DeltaSecond;
	if (IsLoop() == true)
		return fmod(CurrentTime, GetAnimationLength());
	if (CurrentTime > AnimFullTime)
		return -1;
	return CurrentTime;
}

int AnimationClip::GetCurrentFrame(float CurrentTime) const
{
	return static_cast<int>(CurrentTime);
}

int AnimationClip::GetNextFrame(float CurrentTime) const
{
	int NextFrame = GetCurrentFrame(CurrentTime) + 1;
	if (NextFrame >= static_cast<int>(GetDuration()))
		NextFrame = bLoop ? NextFrame / static_cast<int>(GetAnimationLength()) : -1;
	return NextFrame;
}

float AnimationClip::GetCurrentFrameTime(float CurrentTime) const
{
	const float CurrentFrame = static_cast<float>(GetCurrentFrame(CurrentTime));
	return CurrentFrame * TickPerSecond;
}

float AnimationClip::GetNextFrameTime(float CurrentTime) const
{
	const float NextFrame = static_cast<float>(GetNextFrame(CurrentTime));
	return NextFrame < 0 ? NextFrame * TickPerSecond : -1.f;
}

const string& AnimationClip::GetName() const
{
	return Name;
}

float AnimationClip::GetPlayRate() const
{
	return PlayRate;
}

float AnimationClip::GetDuration() const
{
	return Duration;
}

float AnimationClip::GetTickPerSecond() const
{
	return TickPerSecond;
}

void AnimationClip::SetLoop(bool bLoop)
{
	this->bLoop = bLoop;
}

bool AnimationClip::IsLoop() const
{
	return bLoop;
}

const Texture* AnimationClip::GetKeyFrameTexture() const
{
	return KeyFrameTexture;
}

void AnimationClip::ReadAnimationAsset
(
	const CSkeletal * InSkeleton,
	const wstring & AnimationAssetPath,
	vector<KeyFrameData *> & OutKeyFrames
)
{
	const wstring Path = W_ANIMATION_PATH + AnimationAssetPath + L".anim";
	ASSERT(Path::IsFileExist(Path), String::Format("File Not Found : %ls", Path).c_str())
	const BinaryReader * BinReader = new BinaryReader(Path);
	Name = BinReader->ReadString();
	Duration = BinReader->ReadFloat();
	TickPerSecond = BinReader->ReadFloat();

	const UINT KeyFrameCount = BinReader->ReadUint();
	OutKeyFrames.assign(KeyFrameCount, nullptr);
	for (UINT i = 0; i < KeyFrameCount; i++)
	{
		OutKeyFrames[i] = new KeyFrameData();
		KeyFrameData * &TargetData = OutKeyFrames[i];
		const string BoneNameInAnimation = BinReader->ReadString();

		const CBone * const Bone = InSkeleton->FindBone(BoneNameInAnimation);
		if (!!Bone)
		{
			TargetData->BoneIndex = Bone->GetBoneIndex();
			TargetData->BoneName = Bone->GetName();
		}
		else
		{
			// 여기서 BoneIndex == -1인 KeyFrame이 의마하는게 뭘까?
			TargetData->BoneIndex = -1;
			TargetData->BoneName = BoneNameInAnimation;
		}
		
		const UINT PosCount = BinReader->ReadUint();
		TargetData->Positions.resize(PosCount);
		if (PosCount > 0)
		{
			const UINT ReadSize = sizeof(FrameDataVec) * PosCount;
			void * Ptr = TargetData->Positions.data();
			BinReader->ReadByte(&Ptr, ReadSize);
		}

		const UINT ScaleCount = BinReader->ReadUint();
		TargetData->Scales.resize(ScaleCount);
		if (ScaleCount > 0)
		{
			const UINT ReadSize = sizeof(FrameDataVec) * ScaleCount;
			void * Ptr = TargetData->Scales.data();
			BinReader->ReadByte(&Ptr, ReadSize);
		}

		const UINT RotCount = BinReader->ReadUint();
		TargetData->Rotations.resize(RotCount);
		if (RotCount > 0)
		{
			const UINT ReadSize = sizeof(FrameDataQuat) * RotCount;
			void * Ptr = TargetData->Rotations.data();
			BinReader->ReadByte(&Ptr, ReadSize);
		}
	}

	SAFE_DELETE(BinReader);
}

void AnimationClip::CreateKeyFrameTable
(
	const CSkeletal * InSkeleton,
	const vector<KeyFrameData *> & InKeyFrames,
	vector<PackedHalfTRS> & OutKeyFrameArray
) const
{
	map<string, KeyFrameData *> KeyFrameSearchTree;
	const UINT KeyFrameCount = InKeyFrames.size();
	for (UINT i = 0; i < KeyFrameCount; i++)
	{
		KeyFrameSearchTree[InKeyFrames[i]->BoneName] = InKeyFrames[i];
	}
	const UINT BoneCount = InSkeleton->GetBoneCount();
	const UINT AnimationLength = static_cast<UINT>(GetAnimationLength());

	OutKeyFrameArray.resize(AnimationLength * BoneCount);
	for (UINT Frame = 0; Frame < AnimationLength; Frame++)
	{
		for (UINT BoneId = 0; BoneId < BoneCount; BoneId++)
		{
			const UINT Index = Frame * BoneCount + BoneId;

			const CBone * const TargetBone = InSkeleton->FindBone(static_cast<int>(BoneId));
			const auto It = KeyFrameSearchTree.find(TargetBone->GetName());
			if (It == KeyFrameSearchTree.cend())
				continue;
			const KeyFrameData * const TargetKeyFrameData = It->second;

			// 현재 Bone에 대한 NodeData를 찾았다면 해당 Bone의 F번쨰 프레임의 TRS를 가져온다.
			// 이 TRS는 Parent-Coordinate기준 정보다.
			
			Vector Translation = TargetKeyFrameData->Positions.size() == 1 ? TargetKeyFrameData->Positions[0].Value : TargetKeyFrameData->Positions[Frame].Value;
			Vector Scale = TargetKeyFrameData->Scales.size()    == 1 ? TargetKeyFrameData->Scales[0].Value    : TargetKeyFrameData->Scales[Frame].Value;
			Quaternion Rotation = TargetKeyFrameData->Rotations.size() == 1 ? TargetKeyFrameData->Rotations[0].Value : TargetKeyFrameData->Rotations[Frame].Value;
			if (TargetBone->IsRootBone() == false)
			{
				// 여기서 만들어진 AnimationMatrix는 ParentNode의 Local-Coordinate가 기준이다.
				Matrix AnimationMatrix = Matrix::CreateFromTRS(Translation, Rotation, Scale);

				const UINT ParentIndex = Frame * BoneCount + TargetBone->GetParentIndex();
				const PackedHalfTRS & ParentTRS = OutKeyFrameArray[ParentIndex]; // W_T_A. 이미 업데이트 된 부모노드의 World-Transform.
				const Matrix & ParentMatrix = Matrix::CreateFromTRS(ParentTRS.Translation, ParentTRS.Rotation, ParentTRS.Scale);
				// 여기서 AnimationMatrix를 Root 기준으로 변환해준다.
				AnimationMatrix = AnimationMatrix * ParentMatrix; // W_T_B = A_T_B * W_T_A

				AnimationMatrix.Decompose(Scale, Rotation, Translation);
				OutKeyFrameArray[Index];
			}
			OutKeyFrameArray[Index] = {
				Vector::GetPackedVectorHalf4(Translation),
				Quaternion::GetPackedVectorHalf4(Rotation),
				Vector::GetPackedVectorHalf4(Scale)
			};
		}
	}
}

void AnimationClip::CreateKeyFrameTexture(const vector<PackedHalfTRS> & InKeyFramesArray)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	constexpr UINT PixelChannel = 3; // TRSDesc는 R16G16B16A16 3개 필요하다.
	constexpr UINT FormatSize = 8; // R16G16B16A16
	const UINT Width = this->Skeleton->GetBoneCount() * PixelChannel;
	const UINT Height = static_cast<UINT>(GetAnimationLength());

	ID3D11Texture2D * KeyFrameTexture2D = nullptr;
	
	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; //16Byte * 4 = 64 Byte
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA InitialTextureData;
	ZeroMemory(&InitialTextureData, sizeof(D3D11_SUBRESOURCE_DATA));
	const UINT RowPitch = FormatSize * Width;
	const UINT PageSize = Width * Height * FormatSize;
	InitialTextureData.pSysMem = InKeyFramesArray.data();
	InitialTextureData.SysMemPitch = RowPitch;
	InitialTextureData.SysMemSlicePitch = PageSize;
	
	const HRESULT Hr = Device->CreateTexture2D(
		&TextureDesc,
		&InitialTextureData,
		&KeyFrameTexture2D
	);
	CHECK(SUCCEEDED(Hr));

	KeyFrameTexture = new Texture(KeyFrameTexture2D, TextureDesc);
	SAFE_RELEASE(KeyFrameTexture2D);
}
