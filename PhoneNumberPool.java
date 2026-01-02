import java.util.LinkedList;
import java.util.List;
import java.util.Queue;


/*The Prompt: "The Connection Pool"
Interviewer: "At Retell, we own a limited set of 10 Twilio Phone Numbers that our AI agents use to make outbound calls.

I want you to design a class called PhoneNumberPool.

Initialize it with a list of available phone numbers (Strings).

acquire(): An agent calls this to get a phone number.

If a number is free, remove it from the pool and return it.

If no numbers are free, the agent must wait (block) until one becomes available.

release(String number): When the call is over, the agent returns the number to the pool so someone else can use it.

Constraint: This must be thread-safe. We have 50 agents trying to make calls simultaneously." */

public class PhoneNumberPool {
    private final Queue<String> availableNumbers;

    public PhoneNumberPool(List<String> initialNumbers) {
        this.availableNumbers = new LinkedList<>(initialNumbers);
    }

    public synchronized String acquire() throws InterruptedException {
        while (this.availableNumbers.isEmpty()) {
            wait();
        }
        return this.availableNumbers.poll();
    }

    public synchronized void release(String number) {
        this.availableNumbers.add(number);
        notifyAll();
    }
}