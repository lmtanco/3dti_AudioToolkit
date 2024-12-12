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

# Añadir etiquetas para f_c, f_min y f_max de todas las bandas
for i, fc in enumerate(frecuencias):
    ax.text(fc, 0.05 + i * 0.04, rf'$f_c={fc}$', horizontalalignment='center')
    ax.text(fmin[i], 0.05 + i * 0.06, rf'$f_{{min}}={fmin[i]:.0f}$', horizontalalignment='center')
    ax.text(fmax[i], 0.05 + i * 0.02, rf'$f_{{max}}={fmax[i]:.0f}$', horizontalalignment='center')

# Generar colores dinámicamente usando un mapa de colores
colormap = cm.get_cmap('viridis', len(frecuencias))
colores = [colormap(i) for i in range(len(frecuencias))]

# Representar las bandas como rectángulos con distintos colores
for i in range(len(frecuencias)):
    rect = plt.Rectangle((fmin[i], 0), fmax[i] - fmin[i], 1, color=colores[i], alpha=0.3)
    ax.add_patch(rect)

# Añadir etiquetas a los ejes
ax.set_xlabel('Frecuencia (Hz)')
ax.set_ylabel('Eje Y')

# Añadir un título
ax.set_title('Ilustración de las bandas de frecuencia')

# Mostrar la gráfica
plt.show()
