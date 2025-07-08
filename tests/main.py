import subprocess
import os
import glob

def run_assembly_file(s_file):
    """Compila y ejecuta un archivo .s, retorna el resultado"""
    try:
        compile_result = subprocess.run(['gcc', s_file, '-o', 'a.out'], 
                                      capture_output=True, text=True)
        
        if compile_result.returncode != 0:
            return f"Error de compilación: {compile_result.stderr}"
        
        run_result = subprocess.run(['./a.out'], 
                                  capture_output=True, text=True, timeout=5)
        
        if run_result.returncode != 0:
            return f"Error de ejecución (código {run_result.returncode}): {run_result.stderr}"
        
        output = run_result.stdout.strip()
        return output if output else "(sin salida)"
        
    except subprocess.TimeoutExpired:
        return "Error: Tiempo de ejecución agotado"
    except Exception as e:
        return f"Error: {str(e)}"

def main():
    s_files = glob.glob("*.s")
    s_files.sort()
    
    if not s_files:
        print("No se encontraron archivos .s en el directorio actual")
        return
    
    print("=== Ejecutando archivos de ensamblador ===\n")
    
    for s_file in s_files:
        input_name = s_file.replace('.s', '')
        
        print(f"{input_name}: ", end="", flush=True)
        
        result = run_assembly_file(s_file)
        print(f'"{result}"')
    
    if os.path.exists('a.out'):
        os.remove('a.out')
    
    print("\n=== Ejecución completada ===")

if __name__ == "__main__":
    main()