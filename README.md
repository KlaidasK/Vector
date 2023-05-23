# Vector

# 1


# 2

Palyginimas kiek vidutiniškai laiko užtrunka  std::vector tipo ir Vector tipo tuščius vektorius užpildyti: 10000, 100000, 1000000, 10000000 ir 100000000 int elementų naudojant push_back() funkciją:

|Int dydis | std::vector | Vector klasė |
|--------- |---------- |---------- |
|10000 | 0.0017345 s | 5.3e-05 s |
|100000 | 0.0015386 s | 0.0005751 s |
|1000000 | 0.007505 s | 0.0036547 s |
|10000000 | 0.0668431 s | 0.0708599 s |
|100000000 | 1.17614 s | 0.648158 s |

# 3

Su (Vector ir std::vector) atminties perskirstymai užpildant 100000000 elementų yra:

|Int dydis | std::vector | Vector klasė |
|--------- |---------- |---------- |
|100000000 | 47 | 28 |

# 4

Analizė su projektu v2.0




















