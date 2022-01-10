print("Hello World!")
import csv
with open('./Multi_Label_dataset/train.csv', newline='') as csvfile:
    data = csv.reader(csvfile, delimiter=' ', quotechar='|')
    for row in data:
        print('dick '.join(row))