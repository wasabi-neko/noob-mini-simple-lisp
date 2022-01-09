#include "type.hpp"
#include "AST.hpp"
#include "runtime.hpp"

extern "C" int yyparse(void);
extern func_t lisp_main;

env_t runtime_env;

int main(int argc, char *argv[]) {
    yyparse();

    // visit(lisp_main.body.ast_body);
    graph_AST(lisp_main.body.ast_body);
    return 0;
}


// void AST_interpreter(func_t *main, env_t *env) {
//     AST_node *rip;
//     // env->push_func(main);

//     return;
// }

// var_t interpret(AST_node *root, env_t *env, bool allow_exp_arg) {
//     int next_func_rbp = rsp;


//     // push all list into stack
//     AST_node *node = root;
//     while (node != NULL) {
//         var_t val;
//         if (node->child != NULL) {      // if the node is a compound expression
//             if (allow_exp_arg) {
//                 val = node;
//             } else {
//                 env->add_frame();
//                 val = interpret(node->child, env, node->child->property);
//             }
//         } else if (node->name != NULL) {
//             val = ask_val(env->top_func, name);
//         } else {
//             val = node->val;
//         }

//         push_stack(val);
//         node = noed->next;
//     }

//     // function evoke
//     env->rbp = next_func_rbp;
//     func_t *func_to_call = env->call_local_var(0);
//     func_t *func_instance = create_runtime_func(func_to_call);
//     env->push_func(func_instance);  // push to stack and set static link and dynamic_link

//     env->add_frame();
//     var_t result;
//     if (func_instance->is_native) {
//         result = func_instance->body.native_body(func_instance, env);   // natives should release frame inside
//     } else {
//         result = interpret(func_instance->body.ast_body, env, true);
//         env->release_frame();
//     }

//     return result;
// }