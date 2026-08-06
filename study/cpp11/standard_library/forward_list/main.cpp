#include <iostream>
#include <forward_list>

class stack
{
public:
    stack() = default;
    ~stack() = default;

    void push(int value)
    {
        list.push_front(value);
    }

    void pop()
    {
        if (!list.empty())
        {
            list.pop_front();
        }
    }

    int top() const
    {
        if (!list.empty())
        {
            return list.front();
        }
        throw std::runtime_error("Stack is empty");
    }

    int size() const
    {
        return std::distance(list.begin(), list.end());
    }

    bool empty() const
    {
        return list.empty();
    }

    void clear()
    {
        list.clear();
    }

private:
    std::forward_list<int> list;
};

int main()
{
    std::forward_list<int> list = {1, 2, 3, 4, 5};
    std::cout << "Elements in the forward_list: ";
    for (const auto& element : list)
    {
        std::cout << element << " ";
    }
    std::cout << std::endl;

    list.clear();
    std::cout << "Elements in the forward_list after clear(): ";
    for (const auto& element : list)
    {
        std::cout << element << " ";
    }
    std::cout << std::endl;

    for (int i = 1; i <= 5; ++i)
    {
        list.push_front(i);
    }
    std::cout << "Elements in the forward_list after push_front(): ";

    list.reverse();
    std::cout << "Elements in the forward_list after reverse(): ";
    for (const auto& element : list)
    {
        std::cout << element << " ";
    }
    std::cout << std::endl;

    list.insert_after(list.before_begin(), 0);
    list.push_front(-1);

    std::cout << "Elements in the forward_list after insert_after() and push_front(): ";
    for (const auto& element : list)
    {
        std::cout << element << " ";
    }
    std::cout << std::endl;

    stack st;
    st.push(1);
    st.push(2);
    st.push(3);
    std::cout << "Top element of the stack: " << st.top() << std::endl;
    st.pop();
    std::cout << "Top element of the stack after pop(): " << st.top() << std::endl;

    return 0;
}
