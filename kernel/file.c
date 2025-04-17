#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "defs.h"
#include "file.h"

struct devsw devsw[NDEV]; 