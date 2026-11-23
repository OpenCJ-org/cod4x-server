#include <math.h>

static const float MIN_REMAINING_TIME_SECONDS = 0.000001f;
static const float MAX_WALL_RECOVERY_SPEED = 16.0f;
static const float STOCK_WALL_RECOVERY_SPEED = 1.0f;

/* Called only after PM_SlideMove's original repeated-plane recovery adds normal.
 * Keep vertical speed unchanged and leave slopes/floors/ceilings to stock physics.
 */
void OpenCJ_StickyWallRecovery(const float *origin, float *velocity, const float *normal,
                               float remainingTimeSeconds)
{
    if (normal[2] != 0.0f || !isfinite(remainingTimeSeconds) || remainingTimeSeconds < MIN_REMAINING_TIME_SECONDS)
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
    /* Two representable coordinate steps cover endpoint rounding. Cap speed to avoid
     * large impulses when only a tiny fraction of a movement step remains.
     */
    const float recoverySpeed = fminf(MAX_WALL_RECOVERY_SPEED, 2.0f * projectedSpacing / remainingTimeSeconds);
    if (recoverySpeed <= STOCK_WALL_RECOVERY_SPEED)
    {
        return;
    }
    velocity[0] += normal[0] * (recoverySpeed - STOCK_WALL_RECOVERY_SPEED);
    velocity[1] += normal[1] * (recoverySpeed - STOCK_WALL_RECOVERY_SPEED);
}
