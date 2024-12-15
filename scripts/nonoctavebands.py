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

# Definir las frecuencias_audiometria centrales
frecuencias_audiometria = [250, 500, 1000, 2000, 3000, 4000, 6000, 8000]

# Calcular fmin y fmax para cada frecuencia central
fmin = [frecuencias_audiometria[0] / np.sqrt(frecuencias_audiometria[1] / frecuencias_audiometria[0])]
fmax = [frecuencias_audiometria[0] * np.sqrt(frecuencias_audiometria[1] / frecuencias_audiometria[0])]
for i in range(1, len(frecuencias_audiometria)):
    fmin.append(fmax[i - 1])
    if i < len(frecuencias_audiometria) - 1:
        ratio = np.sqrt(frecuencias_audiometria[i + 1] / frecuencias_audiometria[i])
        fmax.append(frecuencias_audiometria[i] * ratio)
    else:
        ratio_last = np.sqrt(frecuencias_audiometria[-1] / frecuencias_audiometria[-2])
        fmax.append(frecuencias_audiometria[-1] * ratio_last)

# Configurar los ticks del eje x para mostrar los valores específicos
xticks = []
for i in range(len(frecuencias_audiometria)):
    xticks.extend([fmin[i], frecuencias_audiometria[i], fmax[i]])
xticks.append(16000)
ax.set_xticks(xticks)
ax.get_xaxis().set_major_formatter(plt.ScalarFormatter())

# Colorear ciertos xticks de un color y otros de otro color
xtick_labels = ax.get_xticklabels()
for label in xtick_labels:
    tick_value = float(label.get_text())
    if tick_value in frecuencias_audiometria:
        label.set_color('blue')
    elif tick_value > max(frecuencias_audiometria) or tick_value < min(frecuencias_audiometria):
        label.set_color('black') 
    else:
        label.set_color('green')  # Color verde para fmin y fmax

# Añadir líneas verticales azules en las fc
for fc in frecuencias_audiometria:
    ax.axvline(x=fc, color='blue', linestyle='--', ymax=0.8)

# Añadir etiquetas para f_c, f_min y f_max de todas las bandas
# for i, fc in enumerate(frecuencias_audiometria):
#     ax.text(fc, 0.05 + i * 0.04, rf'$f_c={fc}$', horizontalalignment='center')
#     ax.text(fmin[i], 0.05 + i * 0.06, rf'$f_{{min}}={fmin[i]:.0f}$', horizontalalignment='center')
#     ax.text(fmax[i], 0.05 + i * 0.02, rf'$f_{{max}}={fmax[i]:.0f}$', horizontalalignment='center')

# Añadir etiquetas y líneas en rojo para valores específicos (los que calcula el software)
valores_especificos = [500, 1000, 1861.2, 2912.95, 4119.534, 5825.9]
for valor in valores_especificos:
    ax.text(valor, 0.45, rf'$f_c={valor}$', horizontalalignment='center', color='red')
    ax.axvline(x=valor, color='red', linestyle='--', ymax=0.4)

# # Añadir valores específicos adicionales (los dos que coinciden)
# valores_adicionales = [500, 1000]
# for valor in valores_adicionales:
#     ax.text(valor, 0.5, rf'$f_c={valor}$', horizontalalignment='center', color='blue')
#     ax.axvline(x=valor, color='blue', linestyle='--', ymax=0.4)

# Generar colores dinámicamente usando un mapa de colores
colormap = cm.get_cmap('viridis', len(frecuencias_audiometria))
colores = [colormap(i) for i in range(len(frecuencias_audiometria))]

# Representar las bandas como rectángulos con distintos colores
for i in range(len(frecuencias_audiometria)):
    # Para la última banda, extender hasta el final del eje x
    if i == len(frecuencias_audiometria) - 1:
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

# Añadir etiquetas a los ejes
ax.set_xlabel('Frequencies of audiometry (Hz)')
ax.set_ylabel('Y Axis')

# Añadir un título
ax.set_title('Illustration of Frequency Bands')

# Crear un segundo eje x en la parte superior
ax_top = ax.twiny()

# Configurar los límites del segundo eje x para que coincidan con los del eje x original
ax_top.set_xlim(ax.get_xlim())

# Configurar el segundo eje x en escala logarítmica
ax_top.set_xscale('log')

# Definir los nuevos xticks para el eje superior
frecuencias_octavebands = [250, 500, 1000, 2000, 4000, 8000, 16000]

# Definir también los límites inferiores y superiores para cada banda de frecuencia de una octava
fmin_octavebands = [frecuencias_octavebands[0] / np.sqrt(frecuencias_octavebands[1] / frecuencias_octavebands[0])]
fmax_octavebands = [frecuencias_octavebands[0] * np.sqrt(frecuencias_octavebands[1] / frecuencias_octavebands[0])]
for i in range(1, len(frecuencias_octavebands)):
    fmin_octavebands.append(fmax_octavebands[i - 1])
    if i < len(frecuencias_octavebands) - 1:
        ratio = np.sqrt(frecuencias_octavebands[i + 1] / frecuencias_octavebands[i])
        fmax_octavebands.append(frecuencias_octavebands[i] * ratio)
    else:
        ratio_last = np.sqrt(frecuencias_octavebands[-1] / frecuencias_octavebands[-2])
        fmax_octavebands.append(frecuencias_octavebands[-1] * ratio_last)

# Configurar los ticks del eje superior para mostrar las frecuencias de las bandas de octava, tanto las centrales como los límites
xticks_octavebands = []
for i in range(len(frecuencias_octavebands)):
    xticks_octavebands.extend([fmin_octavebands[i], frecuencias_octavebands[i], fmax_octavebands[i]])
xticks_octavebands.append(16000)
ax_top.set_xticks(xticks_octavebands)
ax_top.get_xaxis().set_major_formatter(plt.ScalarFormatter())

#Forzar otra vez a que los límites del eje superior coincidan con los del eje x original
ax_top.set_xlim(ax.get_xlim())

# Añadir líneas verticales negras en las frecuencias f_min que delimitan las bandas de octava, que salgan del eje superior hacia abajo
for fmin_octaveband in fmin_octavebands:
    ax_top.axvline(x=fmin_octaveband, color='black', linestyle='-', ymax=1, ymin=0)

# Añadir una etiqueta al eje superior
ax_top.set_xlabel('Frecuencies of Octave Bands(Hz)')

# Añadir una leyenda
blue_patch = plt.Line2D([0], [0], color='blue', lw=1, label='In blue, audiometry frequencies')
green_patch = plt.Line2D([0], [0], color='green', lw=1, label='In green, frequency band limits passed to GammatoneMultiBandExpander::SetGroups')
red_patch = plt.Line2D([0], [0], color='red', lw=1, label='In red, group center bands calculated by the software')
black_patch = plt.Line2D([0], [0], color='black', lw=1, label='In black, octave band limits')
legend = ax.legend(handles=[black_patch, blue_patch, green_patch, red_patch], loc='upper center', bbox_to_anchor=(0.1, 1.15), ncol=1, prop={'size': 8})
for text in legend.get_texts():
    if 'blue' in text.get_text():
        text.set_color('blue')
    elif 'red' in text.get_text():
        text.set_color('red')
    elif 'green' in text.get_text():
        text.set_color('green')
    elif 'black' in text.get_text():
        text.set_color('black')

# Mostrar la gráfica
plt.show()
