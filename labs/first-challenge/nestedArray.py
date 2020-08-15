def length(array):
    count = 0
    for x in array:
        if (type(x) == int):
            count+= 1
        elif (type(x) == list):
            count += length(x)
    return count

print(length([1, 2, [3, 4], 5]))