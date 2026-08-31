import pandas as pd
import matplotlib.pyplot as plt
import os

def generar_graficas_fb():
    if not os.path.exists("results/tiempos_fb.csv"):
        print("No se encontró results/tiempos_fb.csv")
        return

    df_fb = pd.read_csv("results/tiempos_fb.csv")
    
    # Gráfica 1: Tiempo FB vs Longitud por Alfabeto
    plt.figure(figsize=(8, 5))
    for alfabeto_tam, group in df_fb.groupby('alfabeto_tam'):
        group = group.sort_values(by='longitud')
        plt.plot(group['longitud'], group['tiempo_fb_us'], marker='o', linestyle='-', label=f'Alfabeto |Σ|={alfabeto_tam}')
    
    plt.title("Módulo FB: Crecimiento Exponencial de Fuerza Bruta")
    plt.xlabel("Longitud de la contraseña (n)")
    plt.ylabel("Tiempo de ejecución (µs)")
    plt.yscale("log")
    plt.grid(True, which="both", ls="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig("results/fb_crecimiento_exponencial.png", dpi=300)
    plt.close()

    # Gráfica 2: Comparativa Fuerza Bruta vs Diccionario (Criterio 11)
    plt.figure(figsize=(8, 5))
    x = range(len(df_fb))
    plt.bar([i - 0.2 for i in x], df_fb['tiempo_fb_us'], width=0.4, label='Fuerza Bruta', color='crimson')
    plt.bar([i + 0.2 for i in x], df_fb['tiempo_dict_us'], width=0.4, label='Diccionario', color='navy')
    
    plt.xticks(x, [f"Inst. {row['instancia']}\n(n={row['longitud']})" for _, row in df_fb.iterrows()])
    plt.title("Módulo FB: Tiempo de Ejecución (FB vs. Diccionario)")
    plt.xlabel("Instancias del equipo")
    plt.ylabel("Tiempo (µs)")
    plt.yscale("log")
    plt.grid(True, which="both", ls="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig("results/fb_vs_diccionario.png", dpi=300)
    plt.close()
    print("Guardadas gráficas de FB: fb_crecimiento_exponencial.png y fb_vs_diccionario.png")

def generar_graficas_bt():
    if not os.path.exists("results/tiempos_bt.csv"):
        print("No se encontró results/tiempos_bt.csv")
        return

    df_bt = pd.read_csv("results/tiempos_bt.csv")
    
    # Gráfica 3: Nodos Explorados (Con Poda vs Sin Poda - Todas las Variantes)
    plt.figure(figsize=(9, 5))
    
    con_poda = df_bt[df_bt['poda_activa'] == True].sort_values(by='longitud')
    sin_poda = df_bt[df_bt['poda_activa'] == False].sort_values(by='longitud')
    
    plt.plot(con_poda['variante'], con_poda['nodos_visitados'], marker='o', linewidth=2, color='green', label='Nodos Visitados (Con Poda)')
    plt.plot(sin_poda['variante'], sin_poda['nodos_visitados'], marker='s', linestyle='--', linewidth=2, color='crimson', label='Nodos Teóricos (Sin Poda)')
    
    plt.title("Módulo BT: Eficiencia de Poda por Variante (Con Poda vs Sin Poda)")
    plt.xlabel("Variante de la Política")
    plt.ylabel("Número de Nodos (Escala Logarítmica)")
    plt.yscale("log")
    plt.xticks(rotation=20)
    plt.grid(True, which="both", ls="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig("results/bt_reduccion_poda_nodos.png", dpi=300)
    plt.close()
    print("Guardada gráfica de BT: bt_reduccion_poda_nodos.png")

if __name__ == "__main__":
    print("Generando gráficas oficiales para el informe técnico...")
    generar_graficas_fb()
    generar_graficas_bt()
    print("¡Proceso completado con éxito!")