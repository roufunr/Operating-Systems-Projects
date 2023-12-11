import random
import sys

# Function to generate N random integers
def generate_random_integers(N):
    random_integers = [random.randint(1, 1000000000) for _ in range(N)]
    return random_integers

# Function to write integers to a text file
def write_integers_to_file(integers, filename):
    with open(filename, 'w') as file:
        for integer in integers:
            file.write(str(integer) + '\n')

if __name__ == "__main__":
    N = 1000000  # Change this value to generate a different number of integers
    filename = "oneMillion.txt"
    
    random_integers = generate_random_integers(N)
    write_integers_to_file(random_integers, filename)
    
    print(f"{N} random integers have been written to {filename}.")
