ByteAddressBuffer   Input  : register(t0); //SRV
RWByteAddressBuffer Output : register(u0); //UAV

struct Case3_Input
{
    uint3 GroupID           : SV_GroupID;
    uint  GroupIndex        : SV_GroupIndex;
    uint3 GroupThreadID     : SV_GroupThreadID;
    uint3 DispathThreadID   : SV_DispatchThreadID;
};

/*
struct OutputDesc
{
    UINT GroupID[3];

    UINT Index;
    UINT ThreadID[3];
    UINT DispatchID[3];

    UINT TotalIndex;
    float Value;
    float Value2;
}; // size : 52
*/

[numthreads(5, 4, 1)] // Dispatch(3,2,1)
void CSMain(Case3_Input input)
{
    uint3 groupID = input.GroupID; //3 * 4
    
    uint index = input.GroupIndex; //4
    uint3 threadID = input.GroupThreadID; //3 * 4
    uint3 dispatchID = input.DispathThreadID; //3 * 4
    
    uint group = (5 * 4 * 1) * (groupID.y * 3 + groupID.x);
    uint outAddress = (group + index) * 52;
    
    
    int inAddress = (group + index) * 8;
    
    uint totalIndex = Input.Load(inAddress + 0); //4    
    float value = asfloat(Input.Load(inAddress + 4)); //4
    float value2 = cos(totalIndex * 0.1f) * 100.f;
    
    Output.Store3(outAddress +  0, asuint(groupID));
    Output.Store(outAddress  + 12, asuint(index));
    Output.Store3(outAddress + 16, asuint(threadID));
    Output.Store3(outAddress + 28, asuint(dispatchID));
    Output.Store(outAddress  + 40, asuint(totalIndex));
    Output.Store(outAddress  + 44, asuint(value));
    Output.Store(outAddress  + 48, asuint(value2));
}