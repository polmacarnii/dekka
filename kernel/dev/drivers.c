#include <dev/drivers.h>
#include <print.h>
#include <types.h>

#define MAX_DRIVERS 32

static struct driver *drivers[MAX_DRIVERS];
static size_t driver_count = 0;

void driver_register(struct driver *drv) {
  if (drv == NULL) {
    print("[FAIL]");
    print(" Cannot register NULL driver\n");
    return;
  }

  if (driver_count >= MAX_DRIVERS) {
    print("[FAIL] ");
    print("Cannot register driver ");
    print(drv->name);
    print(" : Max number of drivers are registered");
    return;
  }
  drivers[driver_count++] = drv;
}

void drivers_init(void) {
  print("Initializing Drivers\n");
  for (size_t i = 0; i < driver_count; i++) {
    struct driver *drv = drivers[i];

    print("[...]");
    print(drv->name);
    print("\n");

    int ret = drv->init();

    if (ret == 0) {
      print("[OK]");
      print(drv->name);
      print("\n");
    } else {
      print("[FAIL]");
      print(drv->name);
      print("\n");
    }
  }
}
