/*
Last Names: Casey, Choi
Student Numbers: 300291133, 3003282805
the user will run the program and will enter a
number on the command line. The program will then create a separate thread that outputs
all the prime numbers less than or equal to the number entered by the user.

usage: java prime <number>
*/


public class prime {
    public static void main(String[] args){
        String inputString;
        int input;

        if (args.length == 0 || args.length > 1){
            System.out.println("Incorrect parameters");
        }
        else{
            inputString = args[0];
            input = Integer.parseInt(inputString);

            PrimeThread p = new PrimeThread(input);

            p.start();
        }
    }
}

class PrimeThread extends Thread {
    int input;

    public PrimeThread(int input){
        this.input = input;
    }

    public void run() {
        for (int i = 0; i <= this.input; i++){
            if (isPrime(i)){
                System.out.println(i);
            }
        }
    }

    public boolean isPrime(int n){ //returns true if n is prime
        if (n <2 ){ //1 isn't prime
            return false;
        }

        else if (n < 4){//2 and 3 are prime
            return true;
        }

        else{
            for (int i = 2; i * i <= n; i++){ //checks 2 to sqrt(n) is a factor of n
                if (n % i == 0){ return false;}
            }
            return true;
        }
    }
}