/* Run from repository root:
 * gcc -m32 -msse2 -mfpmath=sse tests/sticky_walls.c src/opencj_stickywalls.c -lm -o /tmp/opencj-sticky-walls-test
 * /tmp/opencj-sticky-walls-test
 */
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
extern void OpenCJ_StickyWallRecovery(const float*,float*,const float*,float);

int main(void)
{
    const float origin[3] = {-16296, 100312, -24930};
    const float wall[3] = {0.498964429f, -0.866622448f, 0};
    const float before[3] = {13.9f, 6.85f, 50};
    float velocity[3];
    memcpy(velocity, before, sizeof(velocity));
    OpenCJ_StickyWallRecovery(origin, velocity, wall, .004f);
    assert(velocity[0] > before[0] && velocity[1] < before[1]);
    assert(velocity[2] == before[2]);

    /* Floors, ceilings, slopes and even slightly tilted walls stay unchanged. */
    const float excluded[][3] = {{0,0,1},{0,0,-1},{.6f,0,.8f},{.5f,-.866f,.00001f}};
    for (unsigned i=0; i<sizeof(excluded)/sizeof(excluded[0]); ++i) {
        memcpy(velocity, before, sizeof(velocity));
        OpenCJ_StickyWallRecovery(origin, velocity, excluded[i], .004f);
        assert(!memcmp(velocity, before, sizeof(velocity)));
    }

    const float local[3] = {10,20,30};
    memcpy(velocity, before, sizeof(velocity));
    OpenCJ_StickyWallRecovery(local, velocity, wall, .004f);
    assert(!memcmp(velocity, before, sizeof(velocity)));

    const float invalid[] = {0,-1,NAN,INFINITY,.0000001f};
    for (unsigned i=0; i<sizeof(invalid)/sizeof(invalid[0]); ++i) {
        memcpy(velocity, before, sizeof(velocity));
        OpenCJ_StickyWallRecovery(origin, velocity, wall, invalid[i]);
        assert(!memcmp(velocity, before, sizeof(velocity)));
    }

    /* A tiny remaining fraction cannot cause an unbounded horizontal impulse. */
    memcpy(velocity, before, sizeof(velocity));
    OpenCJ_StickyWallRecovery(origin, velocity, wall, .000001f);
    assert(hypotf(velocity[0]-before[0],velocity[1]-before[1]) < 15.01f);
    assert(velocity[2] == before[2]);
    puts("PASS: wall separation, vertical preservation, floor/slope exclusion, near-origin parity, invalid duration, bounded recovery");
    return 0;
}
