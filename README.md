# Smart Showroom + Vehicle Maintenance Management System

A comprehensive console-based C++ application for managing a vehicle showroom's full business operations — sales, rentals, maintenance, insurance, and financial reporting.

## Features

**Vehicle Management**
- Add cars and bikes, either owned by the showroom or listed on commission from third-party owners
- Search, edit, and update vehicle records
- Track vehicle status (Available / Sold / Rented)

**Sales & Rentals**
- Sell vehicles with full invoice generation
- Rent vehicles and process returns with settlement calculations

**Customer Management**
- Customers are auto-registered from any transaction (sale, rental, maintenance, or insurance)

**Maintenance**
- Log maintenance records per vehicle
- View full maintenance history

**Finance & Services**
- Installment plan calculator
- Insurance record management with premium tracking

**Reports & Data**
- Showroom statistics and daily/monthly revenue reports
- Reprint any past invoice, rental bill, settlement, maintenance receipt, or insurance record
- Persistent data storage — all records saved to and loaded from file

**Engineering details**
- Built with object-oriented design (separate classes for vehicles, sales, rentals, maintenance, insurance, customers)
- Custom input validation for integers, floats, CNIC numbers, and dates
- Custom file I/O layer for saving/loading structured data
- PKR currency formatting (Lakh/Crore notation)

## Tech
- C++
- File-based data persistence (no external database)

## How to run
1. Compile with any C++ compiler (e.g. `g++ showroom_management.cpp -o showroom`)
2. Run the executable
3. Follow the on-screen menu (18 options covering the full workflow)
