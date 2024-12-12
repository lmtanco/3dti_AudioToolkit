import matplotlib.pyplot as plt
import numpy as np

# Crear una figura y un eje
fig, ax = plt.subplots()

# Configurar los límites de los ejes
ax.set_xlim(250, 16000)
ax.set_ylim(0, 1)

# Establecer el eje x en escala logarítmica
ax.set_xscale('log')

# Definir las frecuencias centrales
frecuencias = [250, 500, 1000, 2000, 4000, 8000]

# Calcular fmin y fmax para cada frecuencia central
fmin = [fc / np.sqrt(2) for fc in frecuencias]
fmax = [fc * np.sqrt(2) for fc in frecuencias]

# Configurar los ticks del eje x para mostrar los valores específicos
xticks = []
for i in range(len(frecuencias)):
    xticks.extend([fmin[i], frecuencias[i], fmax[i]])
xticks.append(16000)
ax.set_xticks(xticks)
ax.get_xaxis().set_major_formatter(plt.ScalarFormatter())

# Añadir etiquetas para f_c, f_min y f_max de todas las bandas
for i, fc in enumerate(frecuencias):
    ax.text(fc, 0.05 + i * 0.05, rf'$f_c={fc}$', horizontalalignment='center')
    ax.text(fmin[i], 0.05 + i * 0.05, r'$f_{min}$', horizontalalignment='center')
    ax.text(fmax[i], 0.05 + i * 0.05, r'$f_{max}$', horizontalalignment='center')

# Representar las bandas como rectángulos con distintos tonos de gris
colores = ['dimgray', 'gray', 'darkgray', 'lightgray', 'silver', 'gainsboro']
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
