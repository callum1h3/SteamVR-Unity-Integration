#include "csamplecontrollerdriver.h"
#include "basics.h"
#include <string>
#include <math.h>
#include "basics.h"

using namespace vr;

static double cyaw = 0, cpitch = 0, croll = 0;
static double cpX = 0, cpY = 0, cpZ = 0;
static double ct0, ct1, ct2, ct3, ct4, ct5;

static double c2pX = 0, c2pY = 0, c2pZ = 0;

