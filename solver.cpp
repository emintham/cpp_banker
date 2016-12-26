#include <memory>

#include "emm.h"

int main() {

  std::shared_ptr<EMM> emm = std::make_shared<EMM>();

  emm->commandParser(6);

  return 0;

}
