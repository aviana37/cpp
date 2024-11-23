#include <functional>

namespace aviana {

    template <typename R, typename ... ARGS>
    struct function_signature {
        using return_type = R;
        static constexpr auto N_ARGS = sizeof ... (ARGS);
        template <auto I> requires (I < N_ARGS)
        using arg = ARGS ... [I];

        template <typename F>
        static constexpr bool is_match_v =
            std::is_invocable_v<F, ARGS...> &&
            std::is_same_v<std::invoke_result_t<F, ARGS...>, return_type>;
    };

    template <typename F, typename FS>
    concept valid_function = FS:: template is_match_v<F>;

    template <typename F> requires std::movable<F>
    class function {
    public:
        function(F&& func) : op(std::move(func)) {}
        ~function() = default;

        template <typename ... ARGS>
        auto run(ARGS&& ... args) {
            return std::invoke(op, std::forward<ARGS>(args) ...);
        }
        void set(F&& func) {
            op = std::move(func);
        }

        function& operator=(function&& other) {
            op = std::move(other.op);
            return *this;
        }
        template <typename ... ARGS>
        auto operator()(ARGS&& ... args) {
            return run(std::forward<ARGS>(args) ...);
        }

        function& operator=(const function&) = delete;
        function(const F&) = delete;
        function() = delete;

    private:
        F op;
    };

    template <typename FS>
    auto make_function(auto&& op)
    requires valid_function<decltype(op), FS> {
        return std::move(op);
    }
}

