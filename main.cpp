#include "type.hpp"
#include "AST.hpp"

extern "C" int yyparse(void);
extern func_t lisp_main;

void AST_interpreter(func_t *main, env_t *env);

int main(int argc, char *argv[]) {
    yyparse();

    visit(lisp_main.body.ast_body);
    return 0;
}


void AST_interpreter(func_t *main, env_t *env) {
    AST_node *rip;
    // env->push_func(main);

    //?  maybe i can just use longjump to escape this while loop
    while (true) {  // while (!env->func_stack.empty())
        while (rip != NULL) {

            if (true) {     //* !env->top_func->allow_ast_param
                if (rip->child != NULL) {
                    // this node should be call-able 
                    // if (get_func_ptr(rip->val) == NULL) {
                    //     // type check is insdie of get_val function
                    //     // !error
                    // }

                    // env->push_func(get_func_ptr(rip->val))
                    // push rip
                    // push rbp
                    // next_rbp = rsp
                    rip = rip->child;   // prepare child(arguments) to call the function
                }
            }
            // env->data_stack.push(rip->val);
            rip = rip->next;
        }

        // func_top = env->func_stack.top
        // if (func_top->type == release ) {        //* maybe i can just use NULL to prepresent return_function
            // rsp = rbp
            // rbp = stack.pop
            // rip = stack.pop
            // stack.push(env->result)
        // } else {
            // func_stack.pop();
            // func_stack.push(return_func);
            // rbp = next_rbp;
            // if (func_top->is_native) {
                // func_top->body->native_func(env);
            // } else {
                // if (func->have_scope) {
                    // env->push_static_scope(rip->val);
                // }
                // rip = func_top->body->ast_body;
            // }
        // }
    }
    


    return;
}