import java.util.LinkedList;
import java.util.Queue;

public class ConcurrentJobQueue {
    // TODO: Define your queue storage and capacity variable here
    private final Queue<String> queue;
    private final int capacity;

    public ConcurrentJobQueue(int capacity) {
        this.queue = new LinkedList<>();
        this.capacity = capacity;
    }

    // TODO: Implement the Producer logic
    // 1. Wait if queue is full
    // 2. Add job
    // 3. Notify consumers
    public synchronized void add(String job) throws InterruptedException {
        while (this.queue.size() == this.capacity) {
            wait(); 
        }
        this.queue.add(job);
        notifyAll();
    }

    public synchronized String take() throws InterruptedException {
        while (this.queue.isEmpty()) {
            wait();
        }
        String job = this.queue.poll();
        notifyAll();
        return job;
    }

    public static void main(String[] args) throws InterruptedException {
        ConcurrentJobQueue jobQueue = new ConcurrentJobQueue(2); // Capacity 2

        // Thread 1: Producer (Adds 5 jobs)
        Thread producer = new Thread(() -> {
            try {
                for (int i = 1; i <= 5; i++) {
                    System.out.println("Attempting to add: Job " + i);
                    jobQueue.add("Job " + i);
                    System.out.println("✅ Added: Job " + i);
                    Thread.sleep(100); // Fast producer
                }
            } catch (InterruptedException e) { e.printStackTrace(); }
        });

        // Thread 2: Consumer (Takes 5 jobs)
        Thread consumer = new Thread(() -> {
            try {
                for (int i = 1; i <= 5; i++) {
                    Thread.sleep(1000); // Slow consumer
                    String job = jobQueue.take();
                    System.out.println("⚡ Processed: " + job);
                }
            } catch (InterruptedException e) { e.printStackTrace(); }
        });

        producer.start();
        consumer.start();
        
        producer.join();
        consumer.join();
        System.out.println("--- Test Complete ---");
    }
}