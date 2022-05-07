#ifndef RTMPSERVER_SOCKETEXCEPCION_H
#define RTMPSERVER_SOCKETEXCEPCION_H

#include <exception>
#include <string>

class RTMPHeaderException: public std::exception
{
public:
    /** Constructor (C strings).
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the char* lies
     *                 with the caller.
     */
    explicit RTMPHeaderException(const char* message)
            : msg_(message) {}

    /** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    explicit RTMPHeaderException(std::string  message)
            : msg_(std::move(message)) {}

    /** Destructor.
     * Virtual to allow for subclassing.
     */
    ~RTMPHeaderException() noexcept override = default;

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the Exception object. Callers must
     *          not attempt to free the memory.
     */
    [[nodiscard]] virtual const char* get_error_message() const noexcept {
        return msg_.c_str();
    }

protected:
    /** Error message.
     */
    std::string msg_;
};

#endif //RTMPSERVER_SOCKETEXCEPCION_H
