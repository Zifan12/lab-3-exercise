#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER
#include <cassert>
#include <utility>

class ControlBlockBase {
public:
    // A new control block starts with one owner.
    ControlBlockBase() = default;

    // Deleting through the base pointer must run the derived destructor.
    virtual ~ControlBlockBase() = default;

    // pure virtual function; must be overriden by derived classes
    virtual void* managedAddress() = 0;

    // Delete copies, which also implicitly deletes moves.
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    long increment() {
        // TODO: increment refcount by 1 and return result.
        return ++refcount;
    }

    long decrement() {
        // TODO: decrement refcount by 1 and return result.
        assert(refcount > 0);
        return --refcount;
    }

    long refCount() const {
        // TODO: just return the refcount.
        return refcount;
    }

private:
    // TODO: add field(s) which both control block types need to have
    long refcount = 1;
};

template <typename T>
class ControlBlock : public ControlBlockBase {
public:
    ControlBlock(T* p = nullptr) : mPtr{p} {}
    ~ControlBlock() override {
        delete mPtr;  // overrides the virtual destructor in ControlBlockBase
    }

    void* managedAddress() override { return mPtr; }

private:
    T* mPtr;
};

template <typename T>
class SharedPtr {
public:
    SharedPtr() : stored_pointer{nullptr}, control_block{nullptr} {}
    SharedPtr(T* p) : stored_pointer{p}, control_block{new ControlBlock<T>(p)} {}
    ~SharedPtr() {
        if (control_block != nullptr) {
            if (control_block->decrement() == 0) {
                delete control_block;
            }
        }
    }

    SharedPtr(const SharedPtr& other)
        : stored_pointer{other.stored_pointer}, control_block{other.control_block} {
        // Copying a SharedPtr means we are creating another owner of the same object.
        // We must therefore increase the reference count so the object stays alive
        // for all copies
        if (control_block != nullptr) {
            control_block->increment();
        }
    }

    SharedPtr(SharedPtr&& other)
        : stored_pointer{other.stored_pointer}, control_block{other.control_block} {
        // Move construction transfers ownership without changing the reference
        // count, the moved-from object becomes empty
        other.stored_pointer = nullptr;
        other.control_block = nullptr;
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this == &other) {
            return *this;
        }

        // Reassignment releases the current object if it is the last owner
        if (control_block != nullptr) {
            long count = control_block->decrement();
            if (count == 0) {
                delete control_block;
            }
        }

        stored_pointer = other.stored_pointer;
        control_block = other.control_block;
        if (control_block != nullptr) {
            control_block->increment();
        }

        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        if (this == &other) {
            return *this;
        }

        if (control_block != nullptr) {
            long count = control_block->decrement();
            if (count == 0) {
                delete control_block;
            }
        }

        stored_pointer = other.stored_pointer;
        control_block = other.control_block;

        other.stored_pointer = nullptr;
        other.control_block = nullptr;

        return *this;
    }

    T& operator*() {
        assert(stored_pointer != nullptr);
        return *stored_pointer;
    }

    T* operator->() {
        assert(stored_pointer != nullptr);
        return stored_pointer;
    }

    T* get() { return stored_pointer; }

    bool operator==(const SharedPtr<T>& other) const {
        return other.stored_pointer == stored_pointer;
    }

    operator bool() const { return stored_pointer != nullptr; }

    void swap(SharedPtr<T>& other) {
        std::swap(stored_pointer, other.stored_pointer);
        std::swap(control_block, other.control_block);
    }

    void reset() {
        SharedPtr<T> empty;
        swap(empty);
    }

    void reset(T* other) {
        if (other == stored_pointer) {
            return;
        }
        SharedPtr<T> temp(other);
        swap(temp);
    }

    long useCount() const {
        if (control_block == nullptr) {
            return 0;
        }
        return control_block->refCount();
    }

private:
    T* stored_pointer;
    ControlBlockBase* control_block;
};

template <typename T, typename... Args>
SharedPtr<T> makeSharedBasic(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#endif
