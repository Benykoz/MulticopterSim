/*
   Java Multicopter class

   Uses UDP sockets to communicate with MulticopterSim

   Copyright(C) 2019 Simon D.Levy

   MIT License
 */

import java.lang.Thread;
import java.io.*;
import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.net.InetAddress;

class Multicopter extends Thread {

    private final int TIMEOUT = 1000;

    public Multicopter(String host, int motorPort, int telemetryPort, int motorCount)
    {
        construct(host, motorPort, telemetryPort, motorCount);
    }

    public Multicopter(String host, int motorPort, int telemetryPort)
    {
        construct(host, motorPort, telemetryPort, 4);
    }

    public void run()
    {
        _running = true;

        int count = 0;

        while (_running) {

            getMotors();

            byte [] motorBytes = doublesToBytes(_motorVals);

            DatagramPacket motorPacket = new DatagramPacket(motorBytes, motorBytes.length, _addr, _motorPort);

            try {
                _motorSocket.send(motorPacket);
            }
            catch (Exception e) {
                handleException(e);
            }

            try {
                byte [] telemetryBytes = new byte[8];
                DatagramPacket telemetryPacket = new DatagramPacket(telemetryBytes, telemetryBytes.length, _addr, _telemPort);
                _telemSocket.receive(telemetryPacket);
                double [] telemetryData = bytesToDoubles(telemetryBytes);
                System.out.printf("%f\n", telemetryData[0]);
                count++;
            }
            catch (Exception e) {
                System.out.println("receive: " + e);
            }

            yield();
        }

        _motorSocket.close();
        _telemSocket.close();

    } // run

    private void getMotors()
    {
        for (int i=0; i<_motorVals.length; ++i) {
            _motorVals[i] = 0.6;
        }
    }

    public void halt()
    {
        _running = false;
    }

    private void construct(String host, int motorPort, int telemetryPort, int motorCount)
    {
        _motorPort = motorPort;
        _telemPort = telemetryPort;

        try {
            _addr = InetAddress.getByName(host);
            _motorSocket = new DatagramSocket();
            _telemSocket = new DatagramSocket(telemetryPort);
            _telemSocket.setSoTimeout(TIMEOUT);
        } 
        catch (Exception e) {
            handleException(e);
        }

        _motorVals = new double [motorCount];

        _running = false;
    }


    private int _motorPort;
    private int _telemPort;

    private double [] _motorVals;

    private boolean _running;

    InetAddress _addr;

    public DatagramSocket _motorSocket;
    public DatagramSocket _telemSocket;

    private static void handleException(Exception e)
    {
    }

    // Adapted from view-source:https://stackoverflow.com/questions/2905556/how-can-i-convert-a-byte-array-into-a-double-and-back

    private static byte[] doublesToBytes(double [] doubles)
    {
        int n = doubles.length;

        byte [] bytes = new byte[8*n];

        for (int i=0; i<n; ++i) {

            long l = Double.doubleToRawLongBits(doubles[i]);

            for (int j=0; j<8; ++j) {
                bytes[i*8+j] = (byte)((l >> (j*8)) & 0xff);
            }
        }

        return bytes;
    }

    private static double[] bytesToDoubles(byte [] bytes)
    {
        int n = bytes.length>>3;

        double [] doubles = new double [n];

        for (int i=0; i<n; ++i) {

            long bits = 0;

            for (int j=0; j<8; ++j) {
                bits = (bits << 8) | (bytes[8-j-1] & 0xff);
            }

            doubles[i] = Double.longBitsToDouble(bits);
        }

        return doubles;
    }

    public static void main(String [] args)
    {
        Multicopter copter = new Multicopter("127.0.0.1", 5000, 5001);

        copter.start();

        try {
            Thread.sleep(10000);
        }
        catch (Exception e) {
        }

        copter.halt();
    }
}
