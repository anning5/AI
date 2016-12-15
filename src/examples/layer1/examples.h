////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// include all examples

#include "../../platform/platform.h"
#include <time.h>
#include "a_star/a_star.h"
#include "a_star/map_generator.h"
#include "a_star/a_star_app.h"

namespace octet {
  static app *app_factory(const char *name, int argc, char **argv) {
	  return new a_star_app(argc, argv);
  }

  inline void run_examples(int argc, char **argv) {
    app::init_all(argc, argv);

    if (argc == 1) {
      printf("running triangle... Why not try texture, gif, ping, cube, duck, bump or physics?\n");
      app *myapp = app_factory("triangle", argc, argv);
      // if you can't edit the debug arguments,
      // change the string above to another of the examples.
      myapp->init();
    } else {
      for (int i = 1; i != argc; ++i) {
        if (argv[i][0] != '-') {
          app *myapp = app_factory(argv[i], argc, argv);
          if (myapp) {
            myapp->init();
          }
        }
      }
    }

    app::run_all_apps();
  }
}
