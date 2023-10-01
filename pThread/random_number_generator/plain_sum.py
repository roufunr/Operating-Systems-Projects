total_sum = 0

# Open the file for reading
with open('./../PA2_Rouf_Abdur/oneMillion.txt', 'r') as file:
    for line in file:
        # Convert the line to an integer and add it to the total sum
        total_sum += int(line)

# Print the total sum
print("Sum of one million integers:", total_sum)
