using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Utility
{
    public class Utility
    {
        public static T[] RemoveArrayElement<T>(T[] a, int index)
        {
            T[] a2 = new T[a.Length - 1];

            for (int j = 0; j < index; j++)
                a2[j] = a[j];
            for (int j = index + 1; j < a.Length; j++)
                a2[j - 1] = a[j];

            return a2;
        }

        public static T[] InsertArrayElement<T>(T[] a, int index, T val)
        {
            T[] a2 = new T[a.Length + 1];

            for (int j = 0; j < index; j++)
                a2[j] = a[j];

            a[index] = val;

            for (int j = index; j < a.Length; j++)
                a2[j + 1] = a[j];

            return a2;
        }

		public static int MatToVector(int x, int y, int columns)
		{
			return (y * columns) + x;
		}
    }
}
