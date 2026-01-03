/* LC - https://leetcode.com/problems/print-foobar-alternately/description

Suppose you are given the following code:

class FooBar {
  public void foo() {
    for (int i = 0; i < n; i++) {
      print("foo");
    }
  }

  public void bar() {
    for (int i = 0; i < n; i++) {
      print("bar");
    }
  }
}
The same instance of FooBar will be passed to two different threads:

thread A will call foo(), while
thread B will call bar().
Modify the given program to output "foobar" n times.

Example 1:

Input: n = 1
Output: "foobar"
Explanation: There are two threads being fired asynchronously. One of them calls foo(), while the other calls bar().
"foobar" is being output 1 time.
Example 2:

Input: n = 2
Output: "foobarfoobar"
Explanation: "foobar" is being output 2 times.
*/


class FooBar {
private:
    int n;
    bool next;
    mutex mtx;
    condition_variable cv;

public:
    FooBar(int n) {
        this->n = n;
        this->next = true;
    }

    void foo(function<void()> printFoo) {
        unique_lock<mutex> lock(mtx);
        for (int i = 0; i < n; i++) {
            while(!next){
                cv.wait(lock);
            }
        	// printFoo() outputs "foo". Do not change or remove this line.
        	printFoo();
            next = false;
            cv.notify_all();
        }
    }

    void bar(function<void()> printBar) {
        unique_lock<mutex> lock(mtx);
        for (int i = 0; i < n; i++) {
            while(next){
                cv.wait(lock);
            }
        	// printBar() outputs "bar". Do not change or remove this line.
        	printBar();
            next = true;
            cv.notify_all();
        }
    }
};