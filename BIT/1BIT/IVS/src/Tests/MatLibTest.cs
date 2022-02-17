/**************************************************
 * Názov projektu: Tests
 * Súbor: MatLibTest.cs
 * Dátum: 10.4.2018
 * Posledná zmena: 23.4.2018
 * Autor: Matej Novák xnovak2f(at)fit.vutbr.cz
 * 
 * Popis: Testy matematickej knižnice
 * 
 ***************************************************
/**
 * @file MatLibTest.cs
 * 
 * @brief Trieda MatLibTest - Testy matematickej knižnice
 * @author Matej Novák (xnovak2f)
 * 
 * @note .NET Framework v4.5
 */

using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Calculator;

namespace Tests
{
    /**
     * @class Trieda testov matematických operácií
     */
    [TestClass]
    public class MatLibTest
    {
        private const double Accuracy = 1e-7; /*!< Konštanta určujúca povolenú odchylku */

        /**
         * @test Testuje sčítanie dvoch reálnych čísel
         */
        [TestMethod]
        public void ShouldAdd()
        {
            Assert.AreEqual(9, MatLib.add(4, 5));
            Assert.AreEqual(0, MatLib.add(-5, 5));
            Assert.AreEqual(0, MatLib.add(0, 0));
            Assert.AreEqual(100000000, MatLib.add(60000000, 40000000));
            Assert.AreEqual(1, MatLib.add(0.4, 0.6), Accuracy);
            Assert.AreEqual(11.41676488, MatLib.add(4.4893192, 6.92744568), Accuracy);
            Assert.AreEqual(-11.41676488, MatLib.add(-4.4893192, -6.92744568), Accuracy);

            Assert.AreNotEqual(25, MatLib.add(0, -25));
            Assert.AreNotEqual(0, MatLib.add(-5, -5));
            Assert.AreNotEqual(0, MatLib.add(0.00001, 0.00001));
            Assert.AreNotEqual(11.41676488, MatLib.add(4.9993192, 6.99944568), Accuracy);
        }

        /**
         * @test Testuje odčítanie dvoch reálnych čísel (druhého od prvého)
         */
        [TestMethod]
        public void ShouldSubtract()
        {
            Assert.AreEqual(10, MatLib.subtract(10, 0));
            Assert.AreEqual(-10, MatLib.subtract(0, 10));
            Assert.AreEqual(0, MatLib.subtract(5, 5));
            Assert.AreEqual(0, MatLib.subtract(0, 0));
            Assert.AreEqual(20000000, MatLib.subtract(60000000, 40000000));
            Assert.AreEqual(-0.2, MatLib.subtract(0.4, 0.6), Accuracy);
            Assert.AreEqual(0.2, MatLib.subtract(-0.4, -0.6), Accuracy);
            Assert.AreEqual(-2.43812648, MatLib.subtract(4.4893192, 6.92744568), Accuracy);
            Assert.AreEqual(2.43812648, MatLib.subtract(-4.4893192, -6.92744568), Accuracy);

            Assert.AreNotEqual(0, MatLib.subtract(10, -10));
            Assert.AreNotEqual(0, MatLib.subtract(-10, 10));
            Assert.AreNotEqual(10, MatLib.subtract(-10, 20));
            Assert.AreNotEqual(-50, MatLib.subtract(0, -50.0000357));
        }

        /**
         * @test Testuje násobenie dvoch reálnych čísel
         */
        [TestMethod]
        public void ShouldMultiply()
        {
            Assert.AreEqual(1, MatLib.multiply(1, 1));
            Assert.AreEqual(0, MatLib.multiply(1, 0));
            Assert.AreEqual(0, MatLib.multiply(100000, 0));
            Assert.AreEqual(0, MatLib.multiply(0, 100000));
            Assert.AreEqual(4, MatLib.multiply(2, 2));
            Assert.AreEqual(-4, MatLib.multiply(2, -2));
            Assert.AreEqual(6, MatLib.multiply(-2, -3));
            Assert.AreEqual(625, MatLib.multiply(25, 25));
            Assert.AreEqual(1.805375, MatLib.multiply(1.625, 1.111), Accuracy);
            Assert.AreEqual(-1.805375, MatLib.multiply(-1.625, 1.111), Accuracy);
            Assert.AreEqual(-1.805375, MatLib.multiply(1.625, -1.111), Accuracy);
            Assert.AreEqual(1.805375, MatLib.multiply(-1.625, -1.111), Accuracy);
            Assert.AreEqual(0.0009, MatLib.multiply(9, 0.0001), Accuracy);


            Assert.AreNotEqual(100, MatLib.multiply(10, -10));
            Assert.AreNotEqual(0.13, MatLib.multiply(0.4, 0.3));
            Assert.AreNotEqual(-10000, MatLib.multiply(-100, -100));
            Assert.AreNotEqual(-0.002, MatLib.multiply(0.0002, -0.1), Accuracy);
        }

        /**
         * @test Testuje operáciu delenia dvoch reálnych čísel (prvého druhým)
         */
        [TestMethod]
        public void ShouldDivide()
        {
            try
            {
                MatLib.divide(1, 0);
                Assert.Fail("Chyba, delenie nulou.");
            }
            catch
            {
            }

            try
            {
                MatLib.divide(-549.41268, 0);
                Assert.Fail("Chyba, delenie nulou.");
            }
            catch
            {
            }

            Assert.AreEqual(1, MatLib.divide(1, 1));
            Assert.AreEqual(100, MatLib.divide(100, 1));
            Assert.AreEqual(0.01, MatLib.divide(1, 100), Accuracy);
            Assert.AreEqual(5, MatLib.divide(50, 10));
            Assert.AreEqual(-5, MatLib.divide(50, -10));
            Assert.AreEqual(5, MatLib.divide(-50, -10));
            Assert.AreEqual(0, MatLib.divide(0, 10));
            Assert.AreEqual(10, MatLib.divide(5, 0.5), Accuracy);
            Assert.AreEqual(100, MatLib.divide(1, 0.01), Accuracy);
            Assert.AreEqual(0.6480482716, MatLib.divide(4.4893192, 6.92744568), Accuracy);

            Assert.AreNotEqual(0.6, MatLib.divide(4.4893192, 6.92744568), Accuracy);
            Assert.AreNotEqual(0.4, MatLib.divide(0.1601, 4));
            Assert.AreNotEqual(10, MatLib.divide(0, 10));
        }
    
        /**
         * @test Testuje operáciu vytvorenia faktoriálú prirodzeného čísla
         */
        [TestMethod]
        public void ShouldFactorial()
        {
            Assert.AreEqual(120, MatLib.factorial(5));
            Assert.AreEqual(1, MatLib.factorial(1));
            Assert.AreEqual(3628800, MatLib.factorial(10));
            Assert.AreEqual(1, MatLib.factorial(0));
            Assert.AreNotEqual(120.00001, MatLib.factorial(5));

            try
            {
                MatLib.factorial(-1);
                Assert.Fail("Chyba, faktoriál musí byť minimálne 0.");
            }
            catch
            {
            }
        }

        /**
         * @test Testuje operáciu vytvorenia druhej mocniny reálneho čísla
         */
        [TestMethod]
        public void ShouldSquare()
        {
            Assert.AreEqual(1, MatLib.square(1));
            Assert.AreEqual(0, MatLib.square(0));
            Assert.AreEqual(1, MatLib.square(-1));
            Assert.AreEqual(10000, MatLib.square(100));
            Assert.AreEqual(1.5625, MatLib.square(1.25), Accuracy);
            Assert.AreEqual(0.010201, MatLib.square(0.101), Accuracy);
            Assert.AreEqual(3, MatLib.square(1.73205080756), Accuracy);

            Assert.AreNotEqual(15, MatLib.square(4));
            Assert.AreNotEqual(16, MatLib.square(4.1), Accuracy);
        }

        /**
         * @test Testuje operáciu umocnenia reálneho čísla na prirodzený exponent
         */
        [TestMethod]
        public void ShouldPower()
        {
            Assert.AreEqual(1, MatLib.power(1, 1));
            Assert.AreEqual(1, MatLib.power(1, 10));
            Assert.AreEqual(1, MatLib.power(1, 0));
            Assert.AreEqual(1, MatLib.power(10, 0));
            Assert.AreEqual(1, MatLib.power(0, 0));
            Assert.AreEqual(0, MatLib.power(0, 10));
            Assert.AreEqual(2, MatLib.power(2, 1));
            Assert.AreEqual(1024, MatLib.power(2, 10));
            Assert.AreEqual(27, MatLib.power(3, 3));
            Assert.AreEqual(-27, MatLib.power(-3, 3));
            Assert.AreEqual(81, MatLib.power(-3, 4));
            Assert.AreEqual(2.25, MatLib.power(1.5, 2), Accuracy);
            Assert.AreEqual(915.0625, MatLib.power(5.5, 4), Accuracy);
            Assert.AreEqual(915.0625, MatLib.power(-5.5, 4), Accuracy);
            Assert.AreEqual(-5032.84375, MatLib.power(-5.5, 5), Accuracy);

            Assert.AreNotEqual(105, MatLib.power(10, 3));

            try
            {
                MatLib.power(0, -1);
                Assert.Fail("Chyba, delenie nulou.");
            }
            catch
            {
            }
        }

        /**
         * @test Testuje operáciu vytvorenia druhej odmocniny čísla
         */
        [TestMethod]
        public void ShouldSqrt()
        {
            Assert.AreEqual(0, MatLib.sqrt(0));
            Assert.AreEqual(1, MatLib.sqrt(1));
            Assert.AreEqual(1.414213562373, MatLib.sqrt(2), Accuracy);
            Assert.AreEqual(5, MatLib.sqrt(25));
            Assert.AreEqual(0.707106781186547, MatLib.sqrt(0.5), Accuracy);
            Assert.AreEqual(387.2989801174281, MatLib.sqrt(150000.5), Accuracy);

            Assert.AreNotEqual(1000, MatLib.sqrt(999999), Accuracy);

            try
            {
                MatLib.sqrt(-1);
                Assert.Fail("Chyba, nepatri do oboru realnych cisel.");
            }
            catch
            {
            }
        }

        /**
         * @test Testuje operáciu odmocnenia reálneho čísla celým číslom
         */
        [TestMethod]
        public void ShouldGenroot()
        {
            Assert.AreEqual(0, MatLib.genroot(0, 10));
            Assert.AreEqual(1, MatLib.genroot(1, 1));
            Assert.AreEqual(-2, MatLib.genroot(-8, 3));
            Assert.AreEqual(5, MatLib.genroot(25, 2));
            Assert.AreEqual(100, MatLib.genroot(100000000, 4));
            Assert.AreEqual(3.684031498640386, MatLib.genroot(50, 3), Accuracy);

            Assert.AreNotEqual(1.9, MatLib.genroot(22.7, 5), Accuracy);
            Assert.AreNotEqual(-1.9, MatLib.genroot(-22.7, 5), Accuracy); 

            try
            {
                MatLib.genroot(10, 0);
                Assert.Fail("Chyba, nepatri do oboru realnych cisel.");
            }
            catch
            {
            }

            try
            {
                MatLib.genroot(-8, 2);
                Assert.Fail("Chyba, nepatri do oboru realnych cisel.");
            }
            catch
            {
            }
        }

        /**
         * @test Testuje operáciu vytvorenia logaritmu pri základe 10 z kladného nenulového reálneho čísla
         */
        [TestMethod]
        public void ShouldLog10()
        {
            Assert.AreEqual(1, MatLib.log10(10));
            Assert.AreEqual(6, MatLib.log10(1000000));
            Assert.AreEqual(1.6232492903979, MatLib.log10(42), Accuracy);
            Assert.AreEqual(1.1760912590556, MatLib.log10(15), Accuracy);

            Assert.AreNotEqual(3, MatLib.log10(999), Accuracy);
            Assert.AreNotEqual(1.1, MatLib.log10(11), Accuracy);

            try
            {
                MatLib.log10(-1);
                Assert.Fail("Chyba, nedefinovana hodnota.");
            }
            catch
            {
            }

            try
            {
                MatLib.log10(0);
                Assert.Fail("Chyba, nedefinovana hodnota.");
            }
            catch
            {
            }
        }
    }
}

/*** Koniec súboru MatLibTest.cs ***/

