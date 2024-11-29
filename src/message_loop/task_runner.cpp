#include "ashe/task_runner.h"
#include "ashe/macros.h"

namespace ashe {
namespace {
class DeleteHelper {
   public:
    DeleteHelper(void (*deleter)(const void*), const void* object) :
        deleter_(deleter),
        object_(object) {
        // Nothing
    }

    ~DeleteHelper() {
        doDelete();
    }

    void doDelete() {
        if (deleter_ && object_) {
            deleter_(object_);

            deleter_ = nullptr;
            object_ = nullptr;
        }
    }

   private:
    void (*deleter_)(const void*);
    const void* object_;

    ASHE_DISALLOW_COPY_AND_ASSIGN(DeleteHelper);
};
}  // namespace

void TaskRunner::deleteSoonInternal(void (*deleter)(const void*), const void* object) {
    postNonNestableTask(
        std::bind(&DeleteHelper::doDelete, std::make_shared<DeleteHelper>(deleter, object)));
}
}  // namespace ashe