# nonoctavebands.py
# 
# Este script fue creado con la ayuda de GitHub Copilot.
# 
# Descripción:
# Este script genera u na gráfica con bandas de frecuencia no octavadas,
# mostrando los límites inferiores (fmin) y superiores (fmax) para cada
# frecuencia central (fc). La gráfica utiliza una escala logarítmica en el
# eje x y muestra etiquetas para cada banda de frecuencia.
# 
# Autor: Luis Molina Tanco
# Fecha: Diciembre 2024. 
# Licencia: CC BY-NC-SA 4.0

import matplotlib.pyplot as plt
import numpy as np
import matplotlib.cm as cm

# Crear una figura y un eje
fig, ax = plt.subplots()

# Configurar los límites de los ejes
ax.set_xlim(250, 16000)
ax.set_ylim(0, 1)

# Establecer el eje x en escala logarítmica
ax.set_xscale('log')

# Definir las frecuencias centrales
frecuencias = [250, 500, 1000, 2000, 3000, 4000, 6000, 8000]

# Calcular fmin y fmax para cada frecuencia central
fmin = [frecuencias[0] / np.sqrt(frecuencias[1] / frecuencias[0])]
fmax = [frecuencias[0] * np.sqrt(frecuencias[1] / frecuencias[0])]
for i in range(1, len(frecuencias)):
    fmin.append(fmax[i - 1])
    if i < len(frecuencias) - 1:
        ratio = np.sqrt(frecuencias[i + 1] / frecuencias[i])
        fmax.append(frecuencias[i] * ratio)
    else:
        ratio_last = np.sqrt(frecuencias[-1] / frecuencias[-2])
        fmax.append(frecuencias[-1] * ratio_last)

# Configurar los ticks del eje x para mostrar los valores específicos
xticks = []
for i in range(len(frecuencias)):
    xticks.extend([fmin[i], frecuencias[i], fmax[i]])
xticks.append(16000)
ax.set_xticks(xticks)
ax.get_xaxis().set_major_formatter(plt.ScalarFormatter())

# Colorear ciertos xticks de un color y otros de otro color
xtick_labels = ax.get_xticklabels()
for label in xtick_labels:
    tick_value = float(label.get_text())
    if tick_value in frecuencias:
        label.set_color('blue')
    elif tick_value > max(frecuencias) or tick_value < min(frecuencias):
        label.set_color('black') 
    else:
        label.set_color('green')  # Color verde para fmin y fmax

# Añadir etiquetas para f_c, f_min y f_max de todas las bandas
for i, fc in enumerate(frecuencias):
    ax.text(fc, 0.05 + i * 0.04, rf'$f_c={fc}$', horizontalalignment='center')
    ax.text(fmin[i], 0.05 + i * 0.06, rf'$f_{{min}}={fmin[i]:.0f}$', horizontalalignment='center')
    ax.text(fmax[i], 0.05 + i * 0.02, rf'$f_{{max}}={fmax[i]:.0f}$', horizontalalignment='center')

# Añadir etiquetas y líneas en rojo para valores específicos (los que calcula el software)
valores_especificos = [500, 1000, 1861.2, 2912.95, 4119.534, 5825.9]
for valor in valores_especificos:
    ax.text(valor, 0.5, rf'$f_c={valor}$', horizontalalignment='center', color='red')
    ax.axvline(x=valor, color='red', linestyle='--', ymax=0.4)

# # Añadir valores específicos adicionales (los dos que coinciden)
# valores_adicionales = [500, 1000]
# for valor in valores_adicionales:
#     ax.text(valor, 0.5, rf'$f_c={valor}$', horizontalalignment='center', color='blue')
#     ax.axvline(x=valor, color='blue', linestyle='--', ymax=0.4)

# Generar colores dinámicamente usando un mapa de colores
colormap = cm.get_cmap('viridis', len(frecuencias))
colores = [colormap(i) for i in range(len(frecuencias))]

# Representar las bandas como rectángulos con distintos colores
for i in range(len(frecuencias)):
    # Para la última banda, extender hasta el final del eje x
    if i == len(frecuencias) - 1:
        rect = plt.Rectangle((fmin[i], 0), 16000 - fmin[i], 1, color=colores[i], alpha=0.3)
    else:
        rect = plt.Rectangle((fmin[i], 0), fmax[i] - fmin[i], 1, color=colores[i], alpha=0.3)
    ax.add_patch(rect)

# Añadir una flecha roja con un texto rojo
ax.annotate(r'$f_c = 30000$', xy=(15000, 0.5), xytext=(10000, 0.55),
            color='red', ha='center',
            arrowprops=dict(facecolor='red', edgecolor='red', shrink=0.05, width=0.5, headwidth=5))

# Añadir una flecha roja con un texto rojo
ax.annotate(r'$f_c = 0$', xy=(180, 0.5), xytext=(250, 0.55),
            color='red', ha='center',
            arrowprops=dict(facecolor='red', edgecolor='red', shrink=0.05, width=0.5, headwidth=5))

# Añadir una leyenda
blue_patch = plt.Line2D([0], [0], color='blue', lw=1, label='In blue, audiometry frequencies')
green_patch = plt.Line2D([0], [0], color='green', lw=1, label='In green, frequency band limits passed to GammatoneMultiBandExpander::SetGroups')
red_patch = plt.Line2D([0], [0], color='red', lw=1, label='In red, group center bands calculated by the software')
legend = ax.legend(handles=[blue_patch, green_patch, red_patch], loc='upper center', bbox_to_anchor=(0.2, -0.03), ncol=1)
for text in legend.get_texts():
    if 'blue' in text.get_text():
        text.set_color('blue')
    elif 'red' in text.get_text():
        text.set_color('red')
    elif 'green' in text.get_text():
        text.set_color('green')

# Añadir etiquetas a los ejes
ax.set_xlabel('Frequency (Hz)')
ax.set_ylabel('Y Axis')

# Añadir un título
ax.set_title('Illustration of Frequency Bands')

# Mostrar la gráfica
plt.show()
