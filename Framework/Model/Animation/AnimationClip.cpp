#include "framework.h"
#include "AnimationClip.h"

AnimationClip::AnimationClip
(
	const CSkeletal * InSkeleton,
	const wstring& AnimationAssetPath,
	bool InbLoop
)
	: Skeleton(InSkeleton), bLoop(InbLoop)
{
	ASSERT(!!InSkeleton, "Skeleton Not Valid")
	vector<KeyFrameData *> KeyFrames;
	ReadAnimationAsset(InSkeleton, AnimationAssetPath, KeyFrames);
	vector<TRSDesc> KeyFramesArray;
	CreateKeyFrameTable(InSkeleton, KeyFrames, KeyFramesArray);
	CreateKeyFrameTexture(KeyFramesArray);
}

AnimationClip::~AnimationClip()
{
	SAFE_DELETE(KeyFrameTexture);
}

float AnimationClip::GetNextFrame(float CurrentFrame, float DeltaSecond) const
{
	const float DeltaFrame = DeltaSecond * GetPlayRate() * GetTickPerSecond();
	CurrentFrame += DeltaFrame;
	if (IsLoop() == true)
		return fmod(CurrentFrame, GetAnimationLength());
	if (CurrentFrame > GetAnimationLength())
		return -1;
	return CurrentFrame;
}

float AnimationClip::GetCurrentFrame(float Time) const
{
	if (Time > Duration)
	{
		if (bLoop == false)
			Time = Duration;
		else
			Time = fmod(Time, GetAnimationLength());
	}
	return Time;
}

int AnimationClip::GetKeyFrameCurr(float CurrentTime) const
{
	if (CurrentTime > Duration)
	{
		if (bLoop == false)
			CurrentTime = Duration;
		else
			CurrentTime = fmod(CurrentTime, GetAnimationLength());
	}
	return static_cast<int>(CurrentTime);
}

int AnimationClip::GetKeyFrameNext(float CurrentTime) const
{
	const int Duration = static_cast<int>(GetDuration());
	int NextFrame = GetKeyFrameCurr(CurrentTime) + 1;
	if (NextFrame > Duration)
		NextFrame = bLoop ? NextFrame / static_cast<int>(GetAnimationLength()) : -1;
	return NextFrame;
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
	vector<TRSDesc> & OutKeyFrameArray
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
				const TRSDesc & ParentTRS = OutKeyFrameArray[ParentIndex]; // W_T_A. 이미 업데이트 된 부모노드의 World-Transform.
				const Matrix & ParentMatrix = Matrix::CreateFromTRS(ParentTRS.Translation, ParentTRS.Rotation, ParentTRS.Scale);
				// 여기서 AnimationMatrix를 Root 기준으로 변환해준다.
				AnimationMatrix = AnimationMatrix * ParentMatrix; // W_T_B = A_T_B * W_T_A

				TRSDesc & TRS = OutKeyFrameArray[Index];
				AnimationMatrix.Decompose(TRS.Scale, TRS.Rotation, TRS.Translation);
			}
			else
			{
				OutKeyFrameArray[Index] = {Translation, 0, Rotation, Scale, 0};
			}
		}
	}
}

void AnimationClip::CreateKeyFrameTexture(const vector<TRSDesc> & InKeyFramesArray)
{
	ID3D11Device * const Device = D3D::Get()->GetDevice();
	constexpr UINT PixelChannel = 3; // TRSDesc는 R32G32B32A32 3개 필요하다.
	constexpr UINT FormatSize = 16; // R32G32B32A32
	const UINT Width = this->Skeleton->GetBoneCount() * PixelChannel;
	const UINT Height = static_cast<UINT>(GetAnimationLength());

	ID3D11Texture2D * KeyFrameTexture2D = nullptr;
	
	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	TextureDesc.Width = Width;
	TextureDesc.Height = Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //16Byte * 4 = 64 Byte
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
