# Compilador para Lenguaje C

## 📋 Descripción General

Este proyecto implementa un **compilador completo para un subconjunto del lenguaje C** que traduce código fuente a código ensamblador x86-64. El compilador sigue la arquitectura clásica de compiladores con análisis léxico, sintáctico y generación de código, implementando el patrón Visitor para separar las operaciones de las estructuras de datos del AST.

## 🏗️ Arquitectura del Compilador

```
Código fuente
     ↓
   SCANNER (análisis léxico)
     ↓
   Tokens
     ↓
   PARSER (análisis sintáctico)
     ↓
   AST (Árbol de Sintaxis Abstracta)
     ↓
   GENCODE VISITOR (generación de código)
     ↓
   Código ensamblador x86-64
```

---

## 📁 Componentes del Proyecto

### 🔍 1. VISITOR (visitor.h / visitor.cpp)

**Propósito:** Implementa el patrón Visitor para separar las operaciones de las estructuras de datos del AST.

#### Componentes principales:

**🎯 Clase base `Visitor`:**

- Define una interfaz común con métodos virtuales puros para visitar cada tipo de nodo del AST
- Cada método `visit()` corresponde a un tipo específico de expresión o declaración

**🎯 `PrintVisitor`:**

- **Función:** Recorre el AST e imprime el código fuente reconstruido
- **Uso:** Para debugging y verificar que el parser construyó correctamente el AST
- Maneja indentación automática con `indent_level`

**🎯 `GenCodeVisitor`:**

- **Función:** Genera código ensamblador x86-64 a partir del AST
- **Características:**
  - Gestiona el entorno (`Environment`) para variables y funciones
  - Calcula offsets de memoria para variables locales
  - Genera instrucciones de ensamblador para cada construcción del lenguaje

---

### 🔤 2. TOKEN (token.h / token.cpp)

**Propósito:** Define la unidad básica del análisis léxico.

#### Estructura:

```cpp
class Token {
    Type type;     // Tipo de token (IDENTIFIER, NUMBER, etc.)
    string text;   // Texto original del token
}
```

#### Tipos de tokens soportados:

- **Literales:** `NUMBER`, `STRING_LITERAL`, `CHARACTER_CONSTANT`, `TRUE`, `FALSE`
- **Identificadores:** `IDENTIFIER`
- **Palabras clave:** `INT`, `CHAR`, `VOID`, `BOOL`, `STRUCT`, `IF`, `ELSE`, `WHILE`, `FOR`, `RETURN`, `MAIN`
- **Operadores aritméticos:** `PLUS`, `MINUS`, `MULTIPLY`, `DIVIDE`, `MODULO`, `INCREMENT`, `DECREMENT`
- **Operadores de asignación:** `ASSIGN`, `PLUS_ASSIGN`, `MINUS_ASSIGN`, `MULTIPLY_ASSIGN`, `DIVIDE_ASSIGN`
- **Operadores de comparación:** `EQUAL`, `NOT_EQUAL`, `LESS_THAN`, `GREATER_THAN`, `LESS_EQUAL`, `GREATER_EQUAL`
- **Operadores lógicos:** `LOGICAL_AND`, `LOGICAL_OR`, `LOGICAL_NOT`
- **Operadores de punteros:** `DEREFERENCE`, `POINTER_DECL`, `ADDRESS_OF`, `ARROW`
- **Delimitadores:** `LEFT_PAREN`, `RIGHT_PAREN`, `LEFT_BRACE`, `RIGHT_BRACE`, `LEFT_BRACKET`, `RIGHT_BRACKET`, `SEMICOLON`, `COMMA`, `DOT`
- **Especiales:** `INCLUDE`, `HEADER_NAME`, `PRINTF`, `FORMAT_STRING`

#### Funcionalidades:

- Métodos de verificación: `isType()`, `isOperator()`, `isPreprocessor()`
- Sobrecarga del operador `<<` para depuración

---

### 🔍 3. SCANNER (scanner.h / scanner.cpp)

**Propósito:** Realiza el análisis léxico - convierte texto fuente en tokens.

#### Funcionamiento:

1. **Lectura carácter por carácter** del código fuente
2. **Reconocimiento de patrones** usando autómatas finitos
3. **Generación de tokens** correspondientes

#### Características específicas:

**🎯 Manejo de comentarios:**

- Comentarios de línea: `// comentario`
- Comentarios de bloque: `/* comentario */`

**🎯 Reconocimiento de strings:**

- Strings normales: `"texto"`
- Format strings (para printf): `"Valor: %d"`
- Headers de include: `<stdio.h>` o `"miheader.h"`

**🎯 Operadores compuestos:**

- `++`, `--`, `+=`, `-=`, `*=`, `/=`, `%=`
- `==`, `!=`, `<=`, `>=`, `&&`, `||`
- `->` (acceso a miembro de puntero)

**🎯 Contexto sensible:**

- Distingue entre `*` como multiplicación vs desreferencia
- Reconoce headers después de `#include`
- Maneja el token `else if` como una unidad

---

### 🌳 4. PARSER (parser.h / parser.cpp)

**Propósito:** Realiza el análisis sintáctico - convierte tokens en AST usando gramática descendente recursiva.

#### Arquitectura:

- **Análisis descendente recursivo** con backtracking limitado
- **Precedencia de operadores** manejada por niveles de recursión
- **Lookahead** para distinguir construcciones ambiguas

#### Métodos principales:

**🎯 Expresiones (por precedencia):**

```
parseExpression()
├── parseAssignment()
├── parseLogicalOr()
├── parseLogicalAnd()
├── parseEquality()
├── parseComparison()
├── parseAdditive()
├── parseMultiplicative()
├── parseUnary()
├── parsePostfix()
└── parsePrimary()
```

**🎯 Declaraciones:**

- `parseStatement()` - sentencias generales
- `parseVarDeclaration()` - declaraciones de variables
- `parseFunction()` - definiciones de funciones
- `parseStructDeclaration()` - declaraciones de structs

**🎯 Estructura del programa:**

- `parseProgram()` - entrada principal
- `parseIncludes()` - directivas include
- `parseGlobalDeclarations()` - variables globales
- `parseFunctions()` - lista de funciones
- `parseMainFunction()` - función main

---

### ⚙️ 5. GENCODE (gencode.cpp)

**Propósito:** Implementa `GenCodeVisitor` que traduce el AST a código ensamblador x86-64.

#### Características principales:

**🎯 Gestión de memoria:**

- Calcula stack frames para funciones
- Maneja offsets de variables locales
- Soporte para arrays y structs
- Cálculo automático de tamaños de estructuras

**🎯 Generación de código:**

- **Expresiones:** Genera código para evaluar y dejar resultado en registros
- **Asignaciones:** Maneja lvalues y rvalues correctamente
- **Control de flujo:** Genera etiquetas y saltos para if/while/for
- **Llamadas a función:** Implementa convención de llamadas x86-64
- **Acceso a structs:** Maneja tanto `.` como `->` para acceso a miembros

**🎯 Optimizaciones básicas:**

- Reutilización de registros
- Cálculo de tamaños de stack en tiempo de compilación

---

### 🏗️ 6. EXPRESSION (expression.h / expression.cpp)

**Propósito:** Define todas las clases del AST - la representación interna del programa.

#### Jerarquía principal:

**🎯 Expresiones (`Exp`):**

- `BinaryExp` - operaciones binarias (+, -, \*, ==, etc.)
- `UnaryExp` - operaciones unarias (!, ++, --, \*, &)
- `AssignExp` - asignaciones (=, +=, -=, etc.)
- `NumberExp`, `BoolExp`, `CharExp`, `StringExp` - literales
- `IdentifierExp` - variables
- `FunctionCallExp` - llamadas a función
- `ArrayAccessExp` - acceso a arrays: `arr[index]`
- `MemberAccessExp` - acceso a miembros: `obj.field` o `ptr->field`
- `ParenExp` - expresiones entre paréntesis
- `ArrayInitializerExp` - inicializadores de arrays: `{1, 2, 3}`
- `StructInitializerExp` - inicializadores de structs: `{.x = 1, .y = 2}`

**🎯 Declaraciones (`Stm`):**

- `VarDec` - declaración de variables
- `VarDecList` - lista de declaraciones de variables
- `GlobalVarDec` - declaración de variables globales
- `Function` - definición de función
- `MainFunction` - función main especial
- `StructDeclaration` - definición de struct
- `IfStatement`, `ElseIfStatement` - estructuras condicionales
- `WhileStatement`, `ForStatement` - bucles
- `ReturnStatement` - retorno de función
- `PrintfStatement` - llamadas a printf
- `ExpressionStatement` - expresiones como sentencias

**🎯 Otros:**

- `Type` - información de tipos (int, char\*, struct Persona, etc.)
- `Body` - cuerpo de función o bloque
- `Include` - directivas de preprocesador
- `Parameter`, `ParameterList` - parámetros de funciones
- `Program` - nodo raíz del AST

#### Patrón Visitor:

Cada clase implementa `accept(Visitor* visitor)` que delega la operación al visitor correspondiente.

---

### 🌍 7. ENVIRONMENT (environment.h)

**Propósito:** Maneja el entorno de ejecución - tabla de símbolos y gestión de scopes.

#### Componentes:

**🎯 Información de variables (`VarInfo`):**

```cpp
struct VarInfo {
    int offset;          // Offset en el stack
    string type;         // Tipo de dato
    bool is_pointer;     // Es puntero?
    bool is_array;       // Es array?
    bool is_reference;   // Es referencia?
    bool is_global;      // Es variable global?
    int reg_index;       // Índice de registro
}
```

**🎯 Información de funciones (`FunctionInfo`):**

```cpp
struct FunctionInfo {
    string return_type;                    // Tipo de retorno
    vector<FunctionParamInfo> params;      // Lista de parámetros
    int stack_size;                        // Tamaño del stack frame
}
```

**🎯 Información de structs (`StructInfo`):**

```cpp
struct StructInfo {
    unordered_map<string, FieldInfo> fields;   // Campos del struct
    unordered_map<string, int> offsets;        // Offsets de cada campo
    int size;                                   // Tamaño total del struct
}
```

#### Funcionalidades:

**🎯 Gestión de scopes:**

- `add_level()` / `remove_level()` - crear/eliminar niveles de scope
- `check()` / `lookup()` - buscar variables en la pila de scopes

**🎯 Gestión de funciones:**

- `add_function()` / `get_function()` - registrar y consultar funciones
- Validación de parámetros y tipos de retorno
- Cálculo de stack frames

**🎯 Gestión de structs:**

- `add_struct()` / `get_struct()` - registrar y consultar structs
- Cálculo automático de offsets de miembros
- `get_struct_member_offset()` - obtener offset de un miembro específico

---

## 🎯 Características del Lenguaje Soportado

### Tipos de datos:

- **Primitivos:** `int`, `char`, `bool`, `void`
- **Compuestos:** `struct`, arrays
- **Punteros:** A cualquier tipo
- **Referencias:** Soporte limitado

### Control de flujo:

- **Condicionales:** `if`, `else`, `else if`
- **Bucles:** `while`, `for`
- **Funciones:** Definición, llamadas, parámetros, retorno

### Operadores:

- **Aritméticos:** `+`, `-`, `*`, `/`, `%`
- **Lógicos:** `&&`, `||`, `!`
- **Comparación:** `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Asignación:** `=`, `+=`, `-=`, `*=`, `/=`, `%=`
- **Incremento/Decremento:** `++`, `--` (prefijo y sufijo)
- **Punteros:** `*` (desreferencia), `&` (dirección), `->` (acceso a miembro)

### Estructuras:

- Definición de structs
- Inicialización con sintaxis `{.campo = valor}`
- Acceso a miembros con `.` y `->`

### I/O:

- `printf` para salida con format strings

---

## 🚀 Compilación y Ejecución

### Requisitos:

- Compilador C++ (g++, clang++)
- CMake (opcional)
- Sistema x86-64

### Compilación:

```bash
# Con CMake
mkdir build && cd build
cmake ..
make

# O directamente con g++
g++ -o compiler *.cpp
```

### Uso:

```bash
./compiler archivo.c
```

---

## 📝 Ejemplo Completo

**Código fuente (ejemplo.c):**

```c
#include <stdio.h>

struct Punto {
    int x;
    int y;
};

int main() {
    int a = 5;
    int b = 3;
    struct Punto p = {.x = 10, .y = 20};

    if (a > b) {
        printf("a es mayor: %d\n", a);
    } else {
        printf("b es mayor o igual: %d\n", b);
    }

    printf("Punto: (%d, %d)\n", p.x, p.y);
    return 0;
}
```

**Tokens generados por el Scanner:**

```
INCLUDE('#include') HEADER_NAME('<stdio.h>')
STRUCT('struct') IDENTIFIER('Punto') LEFT_BRACE('{')
INT('int') IDENTIFIER('x') SEMICOLON(';')
INT('int') IDENTIFIER('y') SEMICOLON(';')
RIGHT_BRACE('}') SEMICOLON(';')
...
```

**AST generado por el Parser:**

- Program
  - IncludeList
  - StructDeclarationList
  - MainFunction
    - Body
      - VarDec (a = 5)
      - VarDec (b = 3)
      - VarDec (p = {.x = 10, .y = 20})
      - IfStatement
      - PrintfStatement

**Código ensamblador generado:**

```asm
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp

    movq $5, -8(%rbp)    # a = 5
    movq $3, -16(%rbp)   # b = 3
    # ... resto del código
```

---

## 🔧 Estructura de Archivos

```
compi/
├── README.md              # Este archivo
├── CMakeLists.txt         # Configuración de CMake
├── main.cpp               # Punto de entrada del compilador
├── scanner.h/.cpp         # Analizador léxico
├── token.h/.cpp           # Definición de tokens
├── parser.h/.cpp          # Analizador sintáctico
├── expression.h/.cpp      # Clases del AST
├── visitor.h/.cpp         # Patrón Visitor
├── gencode.cpp           # Generación de código
├── environment.h         # Gestión de símbolos
├── tests/                # Casos de prueba
└── cmake-build-debug/    # Archivos de compilación
```

---

## 🎯 Características Técnicas

- **Patrón Visitor:** Separación limpia entre estructura y operaciones
- **Gestión de memoria:** Stack frames calculados automáticamente
- **Tabla de símbolos:** Soporte para múltiples scopes
- **Generación de código:** Ensamblador x86-64 optimizado
- **Manejo de errores:** Detección en análisis léxico y sintáctico
- **Extensibilidad:** Arquitectura modular para agregar nuevas características

---

## 🚧 Limitaciones Actuales

- No soporta funciones recursivas complejas
- Arrays de tamaño dinámico limitados
- Sin optimizaciones avanzadas de código
- Manejo básico de strings
- Sin soporte para unions o enums

---

---

## 🏗️ Implementación Detallada de Características Extensibles

### � **Manejo de Structs**

Los structs son una de las características más avanzadas del compilador, con soporte completo para:

#### **Definición y Registro de Structs:**

```cpp
// En Environment.h
struct StructInfo {
    unordered_map<string, FieldInfo> fields;   // Campos del struct
    unordered_map<string, int> offsets;        // Offsets calculados automáticamente
    int size;                                   // Tamaño total alineado
};
```

**Proceso de compilación de structs:**

1. **Análisis:** El parser reconoce `struct NombreStruct { campos... };`
2. **Registro:** `GenCodeVisitor::visit(StructDeclaration*)` calcula offsets y tamaños
3. **Almacenamiento:** Se guarda en la tabla de símbolos del Environment
4. **Alineación:** Automática a 8 bytes para optimización

#### **Cálculo de Offsets Automático:**

```cpp
// Ejemplo de cálculo en gencode.cpp
int offset = 0;
for (auto member : structDecl->members->vardecs) {
    int member_size = (tipo == "char") ? 1 : 8;
    int alignment = (member_size == 1) ? 1 : 8;
    offset = (offset + alignment - 1) & ~(alignment - 1);  // Alineación
    info.offsets[member_name] = offset;
    offset += member_size;
}
info.size = (offset + 7) & ~7;  // Alineación final del struct completo
```

#### **Inicialización de Structs:**

```cpp
// Sintaxis soportada
struct Punto p = {.x = 10, .y = 20};
```

**Generación de código para inicialización:**

- Se evalúa cada expresión de inicialización
- Se almacena en el offset correspondiente del struct
- Campos no inicializados se ponen a cero automáticamente

#### **Acceso a Miembros:**

- **Sintaxis `.`:** Para structs por valor (`obj.campo`)
- **Sintaxis `->`:** Para structs por puntero (`ptr->campo`)
- **Cálculo:** `dirección_base + offset_del_campo`

**Ejemplo de código generado:**

```asm
# Para p.x (acceso directo)
movq -16(%rbp), %rax    # cargar p.x (offset 0)

# Para ptr->x (acceso por puntero)
movq %rax, %rbx         # dirección del puntero
addq $0, %rbx           # + offset del campo x
movq (%rbx), %rax       # cargar valor
```

---

### 🎯 **Manejo de Punteros**

El sistema de punteros implementa funcionalidad completa con aritmética y desreferencia:

#### **Tipos de Operaciones de Punteros:**

**🎯 Declaración y asignación:**

```c
int* ptr;           // Declaración
ptr = &variable;    // Asignación de dirección
*ptr = 100;         // Desreferencia para escritura
int valor = *ptr;   // Desreferencia para lectura
```

**🎯 Operadores implementados:**

- **`&` (address-of):** Obtiene la dirección de una variable
- **`*` (dereference):** Accede al valor apuntado
- **`->` (member access):** Acceso a miembros de struct por puntero
- **`++`, `--`:** Incremento/decremento de punteros (prefijo y sufijo)

#### **Generación de Código para Punteros:**

**Dirección de variable (`&variable`):**

```asm
leaq -8(%rbp), %rax     # &variable (variable local)
leaq variable(%rip), %rax   # &variable (variable global)
```

**Desreferencia (`*ptr`):**

```asm
movq -16(%rbp), %rax    # cargar dirección del puntero
movq (%rax), %rax       # cargar valor apuntado
```

**Asignación a través de puntero (`*ptr = valor`):**

```asm
# Evaluar valor
movq $100, %rax
movq %rax, %rbx         # guardar valor
# Evaluar puntero
movq -16(%rbp), %rax    # cargar dirección del puntero
movq %rbx, (%rax)       # *ptr = valor
```

#### **Aritmética de Punteros:**

- **Incremento:** `ptr++` mueve el puntero al siguiente elemento
- **Decremento:** `ptr--` mueve el puntero al elemento anterior
- **Tamaño:** Se considera el tamaño del tipo apuntado (8 bytes por defecto)

#### **Punteros y Funciones:**

- **Parámetros por referencia:** `void func(int* param)`
- **Paso de direcciones:** Se pasa `&variable` como argumento
- **Modificación:** La función puede modificar el valor original

---

### 🎨 **Frontend Web con Streamlit**

El proyecto incluye un **frontend interactivo** desarrollado en **Streamlit** que proporciona una interfaz web moderna para usar el compilador.

#### **Características del Frontend:**

**🎯 Interfaz de Usuario:**

- **Editor de código** con resaltado de sintaxis
- **Ejemplos predefinidos** (Hello World, Punteros, Structs, Funciones, etc.)
- **Compilación en tiempo real**
- **Visualización de resultados** en múltiples paneles

**🎯 Funcionalidades:**

- **Compilación:** Integración directa con el compilador C++
- **Ejecución:** Ejecuta el código compilado automáticamente
- **Métricas:** Muestra tiempos de compilación y ejecución
- **Manejo de errores:** Visualización amigable de errores de compilación
- **Código ensamblador:** Muestra el código generado (si está disponible)

#### **Estructura del Frontend:**

**Archivo principal:** `main.py`

```python
# Configuración de Streamlit
st.set_page_config(page_title="Compilador C", page_icon="⚙️", layout="wide")

# Función principal de compilación
def compile_and_run(codigo, compilador_path="./main"):
    # 1. Crear archivo temporal con el código
    # 2. Ejecutar el compilador
    # 3. Capturar resultados y métricas
    # 4. Ejecutar el código compilado
    # 5. Retornar estadísticas completas
```

**🎯 Ejemplos Incluidos:**

1. **Hello World:** Básico printf
2. **Operaciones Básicas:** Aritmética simple
3. **Punteros:** Uso de `*` y `&`
4. **Estructuras:** Definición y uso de structs
5. **Funciones:** Definición y llamadas
6. **Bucles:** For loops con contadores

#### **Uso del Frontend:**

**Instalación y ejecución:**

```bash
# Instalar dependencias
pip install streamlit

# Ejecutar la interfaz web
streamlit run main.py
```

**Interfaz web disponible en:** `http://localhost:8501`

#### **Arquitectura del Frontend:**

```
main.py (Streamlit)
    ↓
Interfaz Web → Código C → ./main (compilador) → Ejecutable → Resultados
    ↑
Visualización ← Métricas ← Errores/Output ← Ejecución
```

---

## 🔧 Extensiones Técnicas Implementadas

### **Sistema de Referencias:**

- Parámetros por referencia en funciones
- Modificación directa de variables originales
- Sintaxis: `void func(int& param)`

### **Arrays Dinámicos:**

- Soporte para arrays con tamaño calculado en runtime
- Inicialización con `{elemento1, elemento2, ...}`
- Acceso por índice con verificación de límites básica

### **Optimizaciones de Generación:**

- **Reutilización de registros:** Minimiza movimientos de memoria
- **Alineación automática:** Stack frames alineados a 16 bytes
- **Cálculo de offsets:** Optimizado para acceso rápido a variables

### **Convención de Llamadas x86-64:**

- **Registros de parámetros:** `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, `%r9`
- **Stack para parámetros adicionales:** Automático
- **Preservación de registros:** Según convención System V ABI
- **Alineación de stack:** Mantenida para llamadas

---

## 🚀 Uso Completo del Sistema

### **Modo Terminal:**

```bash
# Compilación directa
./main programa.c

# Con salida detallada
./main programa.c > salida.s
```

### **Modo Web (Recomendado):**

```bash
# Iniciar servidor web
streamlit run main.py

# Acceder a http://localhost:8501
# Seleccionar ejemplo o escribir código
# Compilar y ejecutar con un clic
```

### **Flujo de Desarrollo:**

1. **Escribir código** en el editor web
2. **Compilar** con el botón correspondiente
3. **Ver métricas** de rendimiento
4. **Analizar código ensamblador** generado
5. **Ejecutar** y ver resultados
6. **Iterar** y mejorar

---

## 🔮 Posibles Extensiones

### **Características del Lenguaje:**

- Soporte para `union` y `enum`
- Tipos `float` y `double` con operaciones
- Arrays multidimensionales
- Strings como tipo primitivo
- Operadores bitwise completos

### **Optimizaciones Avanzadas:**

- **Constant folding:** Evaluación en tiempo de compilación
- **Dead code elimination:** Eliminación de código no utilizado
- **Register allocation:** Asignación inteligente de registros
- **Loop optimization:** Optimizaciones de bucles
- **Inlining:** Expansión de funciones pequeñas

### **Herramientas de Desarrollo:**

- **Debugger integrado:** Puntos de interrupción y inspección
- **Profiler:** Análisis de rendimiento
- **Memory checker:** Detección de leaks y errores
- **IDE completo:** Editor con autocompletado

### **Arquitecturas Objetivo:**

- **ARM64:** Soporte para procesadores ARM
- **RISC-V:** Arquitectura emergente
- **WebAssembly:** Ejecución en navegadores
- **LLVM backend:** Múltiples arquitecturas

### **Análisis Semántico:**

- **Type checker robusto:** Verificación de tipos estricta
- **Flow analysis:** Análisis de flujo de control
- **Static analysis:** Detección de errores estáticos
- **Warning system:** Sistema de advertencias configurable
