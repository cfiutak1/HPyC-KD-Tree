correct_file = open("outputs/17results_fixed.txt", "r")
my_file = open("archive/after_aqs_results.txt", "r")


correct = 0
incorrect = 0


for i in range(10000):
    for j in range(5):
        correct_result = correct_file.readline().split(",")
        my_result = my_file.readline().split(",")
        # print(i, j)
        # print(i, j)
        # print(correct_result, my_result)

        if correct_result[1] == my_result[1]:
            correct += 1
        else:
            print(i)
            exit(1)
            incorrect += 1


    correct_file.readline()
    my_file.readline()


print(correct, "/", correct + incorrect)