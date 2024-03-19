#ifndef LES_NONCOPYABLE_H
#define LES_NONCOPYABLE_H

namespace les {

class Noncopyable {
public:
    Noncopyable(const Noncopyable &)    = delete;
    void operator=(const Noncopyable &) = delete;

protected:
    Noncopyable()  = default;
    ~Noncopyable() = default;
};

} // namespace les

#endif // LES_NONCOPYABLE_H