using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

class FloatPool
{
    private Stack<Single> available;

    public FloatPool(int capacity)
    {
        available = new Stack<Single>(capacity);
        for (int i = 0; i < capacity; i++)
        {
            available.Push(new Single());
        }
    }

    public Single GetFloat()
    {
        if (available.Count == 0)
        {
            for (int i = 0; i < 1 << 20; i++)
            {
                available.Push(new Single());
            }
        }

        return available.Pop();
    }
    public void FreeFloat(Single single)
    {
        available.Push(single);
    }
}
