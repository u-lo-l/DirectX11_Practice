// https://learn.microsoft.com/en-us/windows/win32/direct3d11/direct3d-11-advanced-stages-compute-shader
/*
 *  DX11의 ComputeShader는 메모리 공유와 스레드 동기화기능을 제공하여 
 * 보다 효과적인 병렬 연산을 제공한다.
 *  ID3D11DeviceContext::Dispatch 나 ID3D11DeviceContext::DispatchIndirect를
 * 호출하여 ComputeShader의 명령을 실행할 수 있다.
 * 
 * DX11의 ComputeShader는 DirectComput5.0으로도 불리며 cs_5_0 프로필을 사용한다.
 * - 최대 스레드 수는 Group당 D3D11_CS_THREAD_GROUP_MAX_THREADS_PER_GROUP 이다.(1024)
 * - numthreads의 X와 Y 차원은 D3D11_CS_THREAD_GROUP_MAX_X ,D3D11_CS_THREAD_GROUP_MAX_Y개로 제한된다(각각 1024)
 * - numthreads의 Z차원은 D3D11_CS_THREAD_GROUP_MAX_Z로 제한되며 64다.
 * RWStructuredBuffer, RWByteAddressBuffer, RWTexture1D등을 지원한다.
 * atomic 명령어 사용가능하다.
 * 
 * Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
 * @remarks
 *  Dispatch는 ComputeShader의 명령어를 실행하기 위한 함수이다.
 *  ComputeShader(이하 CS)는 'Thread Group'에서 동작한다.
 *       GPU는 Thread가 한두개가 아니기 때문에 Thread를 개별적으로 관리하는게 아닌 그룹으로 관리한다.
 *      이 때 ThreadGroup에는 N*M*L개의 스레드가 포함될 수 있다. 
 *      이 값을 [numthreads(N,M,L)]로 지정해준다.
 *  ThreadGroup의 Dimension이 (5,5,2)이면 하나의 ThreadGroup에 50개 스레드가 있는것이다.
 *  또한 (4,1,1) 이런식으로 개별 스레드에 접근가능하다.
 *  Dispatch(X,Y,Z)가 호출되면 (X,Y,Z)차원의 T개의 Thread가 undefined order로 실행된다.
 * 
 *   ComputeShader에서 numthreads(10, 8, 3)을 지정한 경우 Dispatch(5,3,2)가 호출되면 어떻게 될까?
 *  각 그룹은 10*8*3=240개의 스레드가 존재하고, Dispatch는 5*3*2=30개의 스레드그룹을 형성한다.
 *  ComputeShader의 input으로 들어가는 ComputeInput구조체를 보면 SV_붙은 변수들이 보인다.
 *  GPU가 알아서 값 채워주는 시스템 변수인데,
 *  - GroupId : 해당 스레드가 속한 스레드 그룹의 위치.
 *  - GroupThreadId : 그룹내에서 해당 스레드의 위치
 *  - DispatchThreadId : 전체 Dispatch공간에서 해당 스레드의 절대 위치
 *  - GroupIndex : 현재 스레드가 그룹 안에서 몇 번쨰 스레드인지.
 // 3D ThreadGroupId 구하는 공식 | numthreads(X, Y, Z)일 떄
    // SV_GroupIndex = (SV_GroupThreadID.Z * X * Y) + (SV_GroupThreadID.Y * X) + SV_GroupThreadID.X;
 *  
*/



// ByteAddressBuffer Input; // SRV
RWByteAddressBuffer Output; // UAV

// Thread는 Group으로 처리된다.
struct Group
{
    uint3 GroupId;
    uint3 GroupThreadId;
    uint3 DispatchThreadId;
    uint GroupIndex;
    // float Result;
};

struct ComputeInput
{
    uint3 GroupId : SV_GROUPID;
    uint3 GroupThreadId : SV_GROUPTHREADID;
    uint3 DispatchThreadId : SV_DISPATCHTHREADID;
    uint GroupIndex : SV_GroupIndex;
};

// numthreads : ThreadGroup의 dimension을 지정한다.
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/sm5-attributes-numthreads
[numthreads(10, 8, 3)] 
void CS(ComputeInput input)
{
    Group group;

    group.GroupId           = asuint(input.GroupId);
    group.GroupThreadId     = asuint(input.GroupThreadId);
    group.DispatchThreadId  = asuint(input.DispatchThreadId);
    group.GroupIndex        = asuint(input.GroupIndex);

    
    // index : 전체 Dispath공간에서 현재 스레드의 1D인덱스를 구하는 과정.
    uint index = group.GroupId.x * 10 * 8 * 3 + input.GroupIndex;
    uint OutAddr = index * 10 * 4; // 10 * 4는 sizeof(Group)

    // uint InAddr = index * 4;
    // group.Result = asfloat(input.Load(InAddr));
    // group.Result = 0.0f;

    Output.Store3(OutAddr + 0 , asuint(group.GroupId));
    Output.Store3(OutAddr + 12 , asuint(group.GroupThreadId));
    Output.Store3(OutAddr + 24 , asuint(group.DispatchThreadId));
    Output.Store(OutAddr + 36 , asuint(group.GroupIndex));
    // Output.Store(OutAddr + 40 , asuint(group.Result));
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}