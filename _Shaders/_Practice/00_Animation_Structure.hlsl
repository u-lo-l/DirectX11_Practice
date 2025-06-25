struct AnimationFrame
{
    uint    Clip;           // 몇 번 째 Clip인지
    float   CurrentTime;
    int     CurrentFrame;   // 그 clip에서 몇 번째 Frame인지
    int     NextFrame;
};

struct BlendingFrame
{
    float BlendingDuration;
    float ElapsedBlendTime;
    float2 Padding;

    AnimationFrame Current;
    AnimationFrame Next;
};

static const int CURR_FRAME = 0;
static const int NEXT_FRAME = 1;
struct InterploateKeyframeParams
{
    int TargetFrame;
    int BoneIndex;
    float Weight;
};