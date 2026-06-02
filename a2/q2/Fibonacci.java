/**
 * Multithreaded Fibonacci generator.
 * Usage: java Fibonacci <count>
 */
public class Fibonacci {
    public static void main(String[] args) {
        if (args.length != 1) {
            System.err.println("Usage: java Fibonacci <number-of-values>");
            System.exit(1);
        }

        int count;
        try {
            count = Integer.parseInt(args[0]);
        } catch (NumberFormatException e) {
            System.err.println("Error: count must be a non-negative integer.");
            System.exit(1);
            return;
        }

        if (count < 0) {
            System.err.println("Error: count must be non-negative.");
            System.exit(1);
        }

        long[] sequence = new long[count];

        Thread worker = new Thread(new FibonacciGenerator(sequence), "fibonacci-worker");
        worker.start();

        try {
            worker.join();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            System.err.println("Error: parent interrupted while waiting for worker.");
            System.exit(1);
        }

        for (int i = 0; i < sequence.length; i++) {
            if (i > 0) {
                System.out.print(' ');
            }
            System.out.print(sequence[i]);
        }
        System.out.println();
    }
}

/** Worker thread logic: fills the shared array with Fibonacci numbers. */
class FibonacciGenerator implements Runnable {
    private final long[] sequence;

    FibonacciGenerator(long[] sequence) {
        this.sequence = sequence;
    }

    @Override
    public void run() {
        int n = sequence.length;
        if (n == 0) {
            return;
        }
        sequence[0] = 0;
        if (n == 1) {
            return;
        }
        sequence[1] = 1;
        for (int i = 2; i < n; i++) {
            sequence[i] = sequence[i - 1] + sequence[i - 2];
        }
    }
}
