import matplotlib.pyplot as plt

ranges = ['[0 - 99] bytes', '[100 - 199] bytes', '[200 - 299] bytes', '[300 - 399] bytes', 
          '[400 - 499] bytes', '[500 - 599] bytes', '[600 - 699] bytes', '[700 - 799] bytes', 
          '[800 - 899] bytes', '[900 - 999] bytes', '[1000 - 1099] bytes', '[1100 - 1199] bytes', 
          '[1200 - 1299] bytes', '[1300 - 1399] bytes', '[1400 - 1499] bytes', '[1500 - 1599] bytes']
counts = [80740, 6477, 2870, 1985, 1296, 1711, 1603, 1048, 871, 1325, 1285, 637, 144545, 166, 54659, 26557]

plt.figure(figsize=(10, 6))
plt.barh(ranges, counts, color='skyblue')
plt.xlabel('Number of Packets')
plt.ylabel('Byte Range')
plt.title('Histogram of Packet Count by Byte Range')
plt.tight_layout()

plt.show()
