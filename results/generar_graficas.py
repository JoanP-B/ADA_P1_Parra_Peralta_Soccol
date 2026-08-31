import os
import matplotlib.pyplot as plt
import pandas as pd

def generar_graficas_fb():
    """Genera y guarda las gráficas oficiales para el módulo de Fuerza Bruta (FB)."""
    if not os.path.exists("results/tiempos_fb.csv"):
        print("No se encontró results/tiempos_fb.csv")
        return

    df_fb = pd.read_csv("results/tiempos_fb.csv")
    
    # Gráfica 1: Tiempo FB vs Longitud por Alfabeto (Filtrando tiempos válidos > 0)
    plt.figure(figsize=(8, 5))
    df_fb_valid = df_fb[df_fb['tiempo_fb_us'] > 0]
    for alfabeto_tam, group in df_fb_valid.groupby('alfabeto_tam'):
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

    # Gráfica 2: Comparativa Fuerza Bruta vs Diccionario
    plt.figure(figsize=(9, 5))
    x = range(len(df_fb))
    plt.bar([i - 0.2 for i in x], df_fb['tiempo_fb_us'], width=0.4, label='Fuerza Bruta', color='crimson')
    plt.bar([i + 0.2 for i in x], df_fb['tiempo_dict_us'], width=0.4, label='Diccionario', color='navy')
    
    plt.xticks(x, [f"{row['instancia']}\n(n={row['longitud']})" for _, row in df_fb.iterrows()], rotation=25, ha='right')
    plt.title("Módulo FB: Tiempo de Ejecución (FB vs. Diccionario)")
    plt.xlabel("Instancias evaluadas")
    plt.ylabel("Tiempo (µs)")
    plt.yscale("log")
    plt.grid(True, which="both", ls="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig("results/fb_vs_diccionario.png", dpi=300)
    plt.close()
    print(" [OK] Gráficas de FB guardadas: fb_crecimiento_exponencial.png y fb_vs_diccionario.png")

def generar_graficas_bt():
    """Genera y guarda las gráficas oficiales para el módulo de Backtracking (BT)."""
    if not os.path.exists("results/tiempos_bt.csv"):
        print("No se encontró results/tiempos_bt.csv")
        return

    df_bt = pd.read_csv("results/tiempos_bt.csv")
    
    # Gráfica 3: Nodos Explorados (Con Poda vs Sin Poda - Ordenado por longitud y variante)
    plt.figure(figsize=(11, 5))
    
    # Ordenar lógicamente por longitud (n) y nombre de variante
    df_bt = df_bt.sort_values(by=['longitud', 'variante'])
    variantes_unicas = df_bt['variante'].unique()
    
    nodos_con_poda = []
    nodos_sin_poda = []
    
    for v in variantes_unicas:
        row_con = df_bt[(df_bt['variante'] == v) & (df_bt['poda_activa'] == True)]
        row_sin = df_bt[(df_bt['variante'] == v) & (df_bt['poda_activa'] == False)]
        
        nodos_con_poda.append(row_con['nodos_visitados'].values[0] if not row_con.empty else 0)
        nodos_sin_poda.append(row_sin['nodos_visitados'].values[0] if not row_sin.empty else 0)

    x_pos = range(len(variantes_unicas))
    
    plt.plot(x_pos, nodos_con_poda, marker='o', linewidth=2, color='green', label='Nodos Visitados (Con Poda)')
    plt.plot(x_pos, nodos_sin_poda, marker='s', linestyle='--', linewidth=2, color='crimson', label='Nodos Teóricos (Sin Poda)')
    
    plt.title("Módulo BT: Eficiencia de Poda por Variante (Con Poda vs Sin Poda)")
    plt.xlabel("Variante de la Política")
    plt.ylabel("Número de Nodos (Escala Logarítmica)")
    plt.yscale("log")
    plt.xticks(x_pos, variantes_unicas, rotation=30, ha='right')
    plt.grid(True, which="both", ls="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig("results/bt_reduccion_poda_nodos.png", dpi=300)
    plt.close()
    print(" [OK] Gráfica de BT guardada: bt_reduccion_poda_nodos.png")

if __name__ == "__main__":
    print("=====================================================")
    print(" GENERADOR OFICIAL DE GRÁFICAS PARA EL INFORME (P1) ")
    print("=====================================================")
    generar_graficas_fb()
    generar_graficas_bt()
    print("\n¡Todas las gráficas se generaron y guardaron en results/ con éxito!")