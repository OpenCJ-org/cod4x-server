#include <math.h>

/* Called only after PM_SlideMove's original repeated-plane recovery adds normal.
 * Keep vertical speed unchanged and leave slopes/floors/ceilings to stock physics.
 */
void OpenCJ_StickyWallRecovery(const float *origin, float *velocity, const float *normal,
                               float remainingTime)
{
    if (normal[2] != 0.0f || !isfinite(remainingTime) || remainingTime < 0.000001f)
    {
        return;
    }

    float projectedSpacing = 0.0f;
    for (int axis = 0; axis < 2; ++axis)
    {
        if (!isfinite(origin[axis]) || !isfinite(normal[axis]))
        {
            return;
        }
        const float spacing = fabsf(nextafterf(origin[axis], INFINITY) - origin[axis]);
        projectedSpacing += fabsf(normal[axis]) * spacing;
    }
    /* Two projected ULPs cover endpoint rounding. Cap recovery speed to avoid
     * large impulses when only a tiny fraction of a movement step remains.
     */
    const float recoverySpeed = fminf(16.0f, 2.0f * projectedSpacing / remainingTime);
    if (recoverySpeed <= 1.0f)
    {
        return;
    }
    velocity[0] += normal[0] * (recoverySpeed - 1.0f);
    velocity[1] += normal[1] * (recoverySpeed - 1.0f);
}
