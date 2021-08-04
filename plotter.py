import matplotlib.pyplot as plt
import csv

hand_tuned_data = []
with open('results-hand-tuned.csv', newline='') as f:
    reader = csv.DictReader(f)
    for r in reader:
        if (None not in r.values()):
            hand_tuned_data.append(r)

optimized_data = []
with open('results-optimized.csv', newline='') as f:
    reader = csv.DictReader(f)
    for r in reader:
        if (None not in r.values()):
            optimized_data.append(r)


plt.subplot(2,1,1)
plt.title("Comparison Between Hand Tuned and Optimized Coefficients")
plt.scatter([float(d['dt']) for d in hand_tuned_data], [float(d['cte']) for d in hand_tuned_data], label="Hand Tuned")
plt.scatter([float(d['dt']) for d in optimized_data], [float(d['cte']) for d in optimized_data], label="Optimized")
plt.legend()

plt.ylabel("Cross Track Error")

plt.subplot(2,1,2)
plt.scatter([float(d['dt']) for d in hand_tuned_data], [float(d['cte_sum']) for d in hand_tuned_data], label="Hand Tuned")
plt.scatter([float(d['dt']) for d in optimized_data], [float(d['cte_sum']) for d in optimized_data], label="Optimized")
plt.legend()
plt.ylabel("Average Cross Track Error")
plt.xlabel("Elapsed Time (seconds)")


plt.show()

