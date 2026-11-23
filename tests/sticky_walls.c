/* Run from repository root:
 * gcc -m32 -msse2 -mfpmath=sse tests/sticky_walls.c src/opencj_stickywalls.c -lm -o /tmp/opencj-sticky-walls-test
 * /tmp/opencj-sticky-walls-test
 */
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

extern void OpenCJ_StickyWallRecovery(const float *origin, float *velocity,
                                     const float *normal, float remainingTimeSeconds);

static const float distantOrigin[3] = {-16296, 100312, -24930};
static const float wallNormal[3] = {0.498964429f, -0.866622448f, 0};
static const float originalVelocity[3] = {13.9f, 6.85f, 50};

static void checkWallSeparation(void)
{
    float velocity[3];
    memcpy(velocity, originalVelocity, sizeof(velocity));
    OpenCJ_StickyWallRecovery(distantOrigin, velocity, wallNormal, 0.004f);
    assert(velocity[0] > originalVelocity[0] && velocity[1] < originalVelocity[1]);
    assert(velocity[2] == originalVelocity[2]);
}

static void checkExcludedSurfaces(void)
{
    /* Floors, ceilings, slopes and even slightly tilted walls stay unchanged. */
    const float excludedNormals[][3] = {
        {0, 0, 1},
        {0, 0, -1},
        {0.6f, 0, 0.8f},
        {0.5f, -0.866f, 0.00001f},
    };
    for (unsigned index = 0; index < sizeof(excludedNormals) / sizeof(excludedNormals[0]); ++index)
    {
        float velocity[3];
        memcpy(velocity, originalVelocity, sizeof(velocity));
        OpenCJ_StickyWallRecovery(distantOrigin, velocity, excludedNormals[index], 0.004f);
        assert(!memcmp(velocity, originalVelocity, sizeof(velocity)));
    }
}

static void checkNearOriginParity(void)
{
    const float localOrigin[3] = {10, 20, 30};
    float velocity[3];
    memcpy(velocity, originalVelocity, sizeof(velocity));
    OpenCJ_StickyWallRecovery(localOrigin, velocity, wallNormal, 0.004f);
    assert(!memcmp(velocity, originalVelocity, sizeof(velocity)));
}

static void checkInvalidDurations(void)
{
    const float invalidDurations[] = {0, -1, NAN, INFINITY, 0.0000001f};
    for (unsigned index = 0; index < sizeof(invalidDurations) / sizeof(invalidDurations[0]); ++index)
    {
        float velocity[3];
        memcpy(velocity, originalVelocity, sizeof(velocity));
        OpenCJ_StickyWallRecovery(distantOrigin, velocity, wallNormal, invalidDurations[index]);
        assert(!memcmp(velocity, originalVelocity, sizeof(velocity)));
    }
}

static void checkBoundedRecovery(void)
{
    float velocity[3];
    memcpy(velocity, originalVelocity, sizeof(velocity));
    OpenCJ_StickyWallRecovery(distantOrigin, velocity, wallNormal, 0.000001f);
    /* At most 15 units/s extra; allow 0.01 for rounding of the wall normal. */
    assert(hypotf(velocity[0] - originalVelocity[0], velocity[1] - originalVelocity[1]) < 15.01f);
    assert(velocity[2] == originalVelocity[2]);
}

int main(void)
{
    checkWallSeparation();
    checkExcludedSurfaces();
    checkNearOriginParity();
    checkInvalidDurations();
    checkBoundedRecovery();
    puts("PASS: wall separation, vertical preservation, excluded surfaces, "
         "near-origin parity, invalid durations and bounded recovery");
    return 0;
}
