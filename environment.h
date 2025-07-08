#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

struct FieldInfo {
    std::string type_name;
    bool is_pointer;
    bool is_array;
};

struct StructInfo {
    std::unordered_map<std::string, FieldInfo> fields; 
    std::unordered_map<std::string, int> offsets;      
    int size = 0;
};
struct VarInfo {
    int offset;
    string type;
    bool is_pointer;
    bool is_array;
    bool is_reference;
    int reg_index = -1;
    bool is_global = false; 
};
struct FunctionParamInfo : public VarInfo {
    std::string name;
    bool is_reference = false; 
    int reg_index;
};
struct FunctionInfo {
    std::string return_type;
    std::vector<FunctionParamInfo> params;
    int stack_size; 
};


class Environment {
private:
    int current_offset = 0;
    vector<unordered_map<string, VarInfo>> levels; 
    unordered_map<string, FunctionInfo> functions;
    unordered_map<string, StructInfo> structs;

    int search_rib(const string& var) {
        int idx = levels.size() - 1;
        while (idx >= 0) {
            if (levels[idx].find(var) != levels[idx].end()) {
                return idx;
            }
            idx--;
        }
        return -1;
    }
public:

    Environment() {}

    void clear() {
        levels.clear();
    }

    int get_current_offset(){
        return current_offset;
    }
    
    void set_current_offset(int offset){
        current_offset = offset;
    }

    void add_level() {
        unordered_map<string, VarInfo> new_level;
        levels.push_back(new_level);
    }

    void add_var(const string& var,
         int offset, 
         const string& type, 
        bool is_ptr = false, 
        bool is_array = false, 
        bool is_reference=false,
        int reg_index = -1,
        bool is_global = false
        ) {
        if (levels.empty()) {
            cout << "Environment sin niveles: no se pueden agregar variables" << endl;
            exit(0);
        }
        VarInfo info = {offset, type, is_ptr, is_array, is_reference, reg_index, is_global};
        levels.back()[var] = info;
    }

    bool remove_level() {
        if (!levels.empty()) {
            levels.pop_back();
            return true;
        }
        return false;
    }

    
    bool check(const string& x) {
        int idx = search_rib(x);
        return (idx >= 0);
    }

    bool isSave(const string& x){
        int idx = search_rib(x);
        if (idx < 0) {
            return false;
        }
        return true;

    }

    VarInfo lookup(const string& x) {
        int idx = search_rib(x);
        if (idx < 0) {
            cout << "Variable no declarada: " << x << endl;
            exit(0);
        }
        return levels[idx][x];
    }

    string lookup_type(const string& x) {
        return lookup(x).type;
    }

    bool typecheck(const string& var, const string& expected_type) {
        string actual_type = lookup_type(var);
        if (actual_type != expected_type) {
            cout << "Error de tipo: se esperaba " << expected_type << " pero se encontró " << actual_type << " para la variable " << var << endl;
            return false;
        }
        return true;
    }

    void add_function(const string& name, const FunctionInfo& info) {
        functions[name] = info;
    }
    bool has_function(const string& name) {
        return functions.find(name) != functions.end();
    }
    FunctionInfo get_function(const string& name) {
        if (!has_function(name)) {
            cout << "Función no declarada: " << name << endl;
            exit(0);
        }
        return functions[name];
    }
    vector<FunctionParamInfo> get_function_params(const string& name) {
        if (!has_function(name)) {
            cout << "Función no declarada: " << name << endl;
            exit(0);
        }
        return functions[name].params;
    }
    int get_function_stack_size(const string& name) {
        if (!has_function(name)) {
            cout << "Función no declarada: " << name << endl;
            exit(0);
        }
        return functions[name].stack_size;
    }
    void set_function_stack_size(const string& name, int size) {
        if (!has_function(name)) {
            cout << "Función no declarada: " << name << endl;
            exit(0);
        }
        functions[name].stack_size = size;
    }
    bool has_function_declared(const string& name) {
        return functions.find(name) != functions.end();
    }
    string get_function_return_type(const string& name) {
        if (!has_function(name)) {
            cout << "Función no declarada: " << name << endl;
            exit(0);
        }
        return functions[name].return_type;
    }
    int get_function_param_offset(const string& func_name, const string& param_name) {
        if (!has_function(func_name)) {
            cout << "Función no declarada: " << func_name << endl;
            exit(0);
        }
        const auto& params = functions[func_name].params;
        for (const auto& param : params) {
            if (param.name == param_name) {
                return param.offset;
            }
        }
        cout << "Parámetro no encontrado: " << param_name << " en función " << func_name << endl;
        exit(0);
    }   

    int get_struct_size(const std::string& name) {
        if (!has_struct(name)) {
            std::cout << "Struct no declarado: " << name << std::endl;
            exit(0);
        }
        return structs[name].size;
    }
    void add_struct(const std::string& name, const StructInfo& info) {
        structs[name] = info;
    }

    int get_struct_member_offset(const string& struct_name, const string& member_name) {
        if (!has_struct(struct_name)) {
            cout << "Struct no declarado: " << struct_name << endl;
            exit(0);
        }
        const auto& info = structs[struct_name];
        if (info.offsets.find(member_name) == info.offsets.end()) {
            cout << "Miembro no encontrado: " << member_name << " en struct " << struct_name << endl;
            exit(0);
        }
        return info.offsets.at(member_name);
    }

    bool has_struct(const string& name) {
        return structs.find(name) != structs.end();
    }

    StructInfo get_struct(const string& name) {
        if (!has_struct(name)) {
            cout << "Struct no declarado: " << name << endl;
            exit(0);
        }
        return structs[name];
    }
};

#endif // ENVIRONMENT_H
