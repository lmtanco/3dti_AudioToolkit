import numpy as np
import matplotlib.pyplot as plt

# Parámetros fijos de ejemplo
bandFrequency = 2000.0
previousBandFrequency = 1000.0

# Rango de expanderFrequency para mostrar la variación
expander_frequencies = np.linspace(previousBandFrequency, bandFrequency, 50)

previous_factors = []
posterior_factors = []

for expanderFrequency in expander_frequencies:
    previousFactor = (bandFrequency - expanderFrequency) / (bandFrequency - previousBandFrequency)
    posteriorFactor = (expanderFrequency - previousBandFrequency) / (bandFrequency - previousBandFrequency)
    previous_factors.append(previousFactor)
    posterior_factors.append(posteriorFactor)

plt.plot(expander_frequencies, previous_factors, label='previousFactor')
plt.plot(expander_frequencies, posterior_factors, label='posteriorFactor')
plt.xlabel('expanderFrequency')
plt.ylabel('Factor')
plt.title('Visualización de previousFactor y posteriorFactor')

# Añadir anotaciones para bandFrequency y previousBandFrequency
plt.annotate(f'bandFrequency = {bandFrequency}', xy=(bandFrequency, 0), xytext=(bandFrequency, 0.5),
             arrowprops=dict(facecolor='black', shrink=0.05))
plt.annotate(f'previousBandFrequency = {previousBandFrequency}', xy=(previousBandFrequency, 0), xytext=(previousBandFrequency, 0.5),
             arrowprops=dict(facecolor='black', shrink=0.05))

# Añadir anotación para un valor concreto de expanderFrequency
expander_value = 1861.21
plt.annotate(f'expanderFrequency = {expander_value}', xy=(expander_value, 0), xytext=(expander_value, 0.6),
             arrowprops=dict(facecolor='red', shrink=0.05))

plt.legend()
plt.grid(True)

# Ajustar los márgenes de la figura
plt.subplots_adjust(left=0.2, right=0.8, top=0.8, bottom=0.2)

plt.show()