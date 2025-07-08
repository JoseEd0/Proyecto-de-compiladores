import streamlit as st
import subprocess
import os
import tempfile
import time
from pathlib import Path

# Configuración básica de la página
st.set_page_config(page_title="Compilador C", page_icon="⚙️", layout="wide")

# CSS simple y limpio
st.markdown(
    """
<style>
    .main {
        padding: 1rem;
    }
    
    .header {
        text-align: center;
        padding: 2rem 0;
        background: #f0f2f6;
        border-radius: 10px;
        margin-bottom: 2rem;
    }
    
    .metric-box {
        background: white;
        border: 1px solid #e0e0e0;
        border-radius: 8px;
        padding: 1rem;
        text-align: center;
        margin: 0.5rem 0;
    }
    
    .success {
        color: #28a745;
        font-weight: bold;
    }
    
    .error {
        color: #dc3545;
        font-weight: bold;
    }
    
    .warning {
        color: #fd7e14;
        font-weight: bold;
    }
    
    .code-area {
        background: #f8f9fa;
        border: 1px solid #e9ecef;
        border-radius: 5px;
        padding: 1rem;
        margin: 1rem 0;
    }
</style>
""",
    unsafe_allow_html=True,
)


def load_examples():
    """Cargar ejemplos de código"""
    examples = {
        "Hello World": """#include <stdio.h>
int main() {
    printf("¡Hola, mundo!\n");
    return 0;
}""",
        "Operaciones Básicas": """#include <stdio.h>
int main() {
    int a = 10;
    int b = 5;
    int suma = a + b;
    int resta = a - b;
    printf("Suma: %d, Resta: %d\n", suma, resta);
    return 0;
}""",
        "Punteros": """#include <stdio.h>
int main() {
    int x = 100;
    int* ptr = &x;
    *ptr = 200;
    printf("Valor de x: %d\n", x);
    return 0;
}""",
        "Estructuras": """#include <stdio.h>
struct Point {
    int x, y;
};

int main() {
    struct Point p = {10, 20};
    p.x = p.x + 5;
    printf("Punto: (%d, %d)\n", p.x, p.y);
    return 0;
}""",
        "Funciones": """#include <stdio.h>
int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(15, 25);
    printf("Resultado: %d\n", result);
    return 0;
}""",
        "Bucles": """#include <stdio.h>
int main() {
    int sum = 0;
    
    for (int i = 1; i <= 5; i++) {
        sum = sum + i;
    }
    
    printf("Suma: %d\n", sum);
    return 0;
}""",
    }
    return examples


def compile_and_run(codigo, compilador_path="./main"):
    """Compilar y ejecutar el código usando el compilador personalizado"""

    stats = {
        "success": False,
        "assembly_code": "",
        "execution_output": "",
        "compilation_time": 0,
        "execution_time": 0,
        "return_code": None,
        "errors": "",
    }

    with tempfile.TemporaryDirectory() as tmpdir:
        try:
            # Crear archivo de código
            txt_path = os.path.join(tmpdir, "programa.txt")
            with open(txt_path, "w", encoding="utf-8") as f:
                f.write(codigo)

            # Compilar con el compilador personalizado
            start_time = time.time()
            result = subprocess.run(
                [compilador_path, txt_path], capture_output=True, text=True, timeout=30
            )
            stats["compilation_time"] = time.time() - start_time

            if result.returncode != 0:
                stats["errors"] = (
                    f"Error de compilación:\n{result.stdout}\n{result.stderr}"
                )
                return stats

            # Leer archivo assembly generado
            asm_path = txt_path.replace(".txt", ".s")
            if os.path.exists(asm_path):
                with open(asm_path, "r", encoding="utf-8") as f:
                    stats["assembly_code"] = f.read()
            else:
                stats["errors"] = "No se generó el archivo assembly"
                return stats

            # Compilar assembly con GCC
            bin_path = os.path.join(tmpdir, "programa_bin")
            gcc_result = subprocess.run(
                ["gcc", "-g", "-no-pie", asm_path, "-o", bin_path],
                capture_output=True,
                text=True,
                timeout=15,
            )

            if gcc_result.returncode != 0:
                stats["errors"] = f"Error en GCC:\n{gcc_result.stderr}"
                return stats

            # Ejecutar el programa
            start_time = time.time()
            run_result = subprocess.run(
                [bin_path], capture_output=True, text=True, timeout=10
            )
            stats["execution_time"] = time.time() - start_time
            stats["execution_output"] = run_result.stdout
            stats["return_code"] = run_result.returncode
            stats["success"] = True

            if run_result.stderr:
                stats["errors"] = f"Errores de ejecución:\n{run_result.stderr}"

            return stats

        except subprocess.TimeoutExpired:
            stats["errors"] = "Timeout: El proceso tardó demasiado tiempo"
            return stats
        except FileNotFoundError:
            stats["errors"] = f"No se encontró el compilador en: {compilador_path}"
            return stats
        except Exception as e:
            stats["errors"] = f"Error inesperado: {str(e)}"
            return stats


def main():
    # Header
    st.markdown(
        """
    <div class="header">
        <h1>⚙️ Compilador C</h1>
        <p>Compilador con soporte para structs, punteros y generación de código x86-64</p>
        <p><em>Hecho por Zamir Lizardo y Matias Meneses para el curso de Compiladores de la UTEC</em></p>
    </div>
    """,
        unsafe_allow_html=True,
    )

    # Sidebar para configuración
    with st.sidebar:
        st.header("⚙️ Configuración")

        compilador_path = st.text_input("Ruta del compilador:", value="./main")

        st.header("📚 Ejemplos")
        examples = load_examples()
        selected_example = st.selectbox(
            "Selecciona un ejemplo:", [""] + list(examples.keys())
        )

        if st.button("Cargar Ejemplo") and selected_example:
            st.session_state.codigo = examples[selected_example]
            st.rerun()

        st.header("ℹ️ Ayuda")
        st.markdown(
            """
        **Instrucciones:**
        1. Escribe tu código C
        2. Haz clic en 'Compilar y Ejecutar'
        3. Revisa los resultados
        
        **Características soportadas:**
        - Variables y operaciones
        - Punteros
        - Estructuras
        - Funciones
        - Bucles y condicionales
        """
        )

    # Contenido principal
    col1, col2 = st.columns([3, 2])

    with col1:
        st.header("📝 Editor de Código")

        # Inicializar código por defecto
        if "codigo" not in st.session_state:
            st.session_state.codigo = """#include <stdio.h>
int main() {
    printf("¡Hola, mundo!\n");
    return 0;
}"""

        # Editor de código
        codigo = st.text_area(
            "Código C:", value=st.session_state.codigo, height=300, key="editor"
        )

        st.session_state.codigo = codigo

        # Botones de control
        col_btn1, col_btn2, col_btn3 = st.columns(3)

        with col_btn1:
            compile_btn = st.button("🚀 Compilar y Ejecutar", type="primary")

        with col_btn2:
            if st.button("🧹 Limpiar"):
                st.session_state.codigo = ""
                st.rerun()

        with col_btn3:
            st.download_button(
                label="💾 Descargar .c",
                data=codigo,
                file_name="programa.c",
                mime="text/plain",
            )

    with col2:
        st.header("📊 Resultados")

        if compile_btn and codigo.strip():
            with st.spinner("Compilando..."):
                stats = compile_and_run(codigo, compilador_path)

            # Mostrar métricas básicas
            col_m1, col_m2 = st.columns(2)

            with col_m1:
                st.markdown(
                    f"""
                <div class="metric-box">
                    <h4>⏱️ Compilación</h4>
                    <p>{stats['compilation_time']:.3f}s</p>
                </div>
                """,
                    unsafe_allow_html=True,
                )

            with col_m2:
                st.markdown(
                    f"""
                <div class="metric-box">
                    <h4>🚀 Ejecución</h4>
                    <p>{stats['execution_time']:.3f}s</p>
                </div>
                """,
                    unsafe_allow_html=True,
                )

            # Estado del programa
            if stats["success"]:
                if stats["return_code"] == 0:
                    st.markdown(
                        '<p class="success">✅ Compilación y ejecución exitosa</p>',
                        unsafe_allow_html=True,
                    )
                else:
                    st.markdown(
                        f'<p class="error">❌ Error en ejecución (código: {stats["return_code"]})</p>',
                        unsafe_allow_html=True,
                    )
            else:
                st.markdown(
                    '<p class="error">❌ Error en compilación</p>',
                    unsafe_allow_html=True,
                )

            # Tabs para resultados
            if stats["success"]:
                tab1, tab2 = st.tabs(["🖥️ Salida", "🔧 Assembly"])

                with tab1:
                    st.subheader("Salida del Programa")
                    if stats["execution_output"]:
                        st.code(stats["execution_output"], language="text")
                    else:
                        st.info("El programa no produjo salida")

                with tab2:
                    st.subheader("Código Assembly Generado")
                    if stats["assembly_code"]:
                        st.code(stats["assembly_code"], language="asm")

                        # Estadísticas del assembly
                        lines = len(stats["assembly_code"].split("\n"))
                        instructions = len(
                            [
                                line
                                for line in stats["assembly_code"].split("\n")
                                if line.strip()
                                and not line.strip().startswith("#")
                                and not line.strip().startswith(".")
                                and ":" not in line
                            ]
                        )

                        st.info(
                            f"📄 Líneas totales: {lines} | 🔧 Instrucciones: {instructions}"
                        )

                        # Descargar assembly
                        st.download_button(
                            label="📥 Descargar Assembly",
                            data=stats["assembly_code"],
                            file_name="programa.s",
                            mime="text/plain",
                        )
                    else:
                        st.warning("No se generó código assembly")

            # Mostrar errores si existen
            if stats["errors"]:
                st.error("**Errores:**")
                st.code(stats["errors"], language="text")

        elif compile_btn and not codigo.strip():
            st.warning("⚠️ Por favor, ingresa código para compilar")

        else:
            st.info(
                """
            👋 **¡Bienvenido!**
            
            Escribe tu código C en el editor y haz clic en 
            "Compilar y Ejecutar" para ver los resultados.
            
            **Características:**
            - Genera código assembly x86-64
            - Soporte para punteros y structs
            - Funciones y bucles
            - Métricas de rendimiento
            """
            )


if __name__ == "__main__":
    main()
