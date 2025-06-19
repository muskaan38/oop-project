# oop-project
# Smart Parking Assistant

## Overview
A C++ application for managing parking slots with user and admin functionalities.

## Features

### User Features:
- Book Parking Slot (manual or automatic)
- View Current Booking
- Exit & Generate Bill

### Admin Features:
- View/Cancel Parking Slots
- Clear Ticket History
- Search Tickets by Date
- Admin Dashboard

## How to Use

1. Compile:
   g++ Smart_Parking_Assisstant_Project.cpp -o parking_system

2. Run:
   ./parking_system

3. Login Options:
   - Admin (password: admin123)
   - User

## Parking Details
- Layout: 4 rows x 5 columns
- Rate: Rs. 50/hour (1 hour minimum)
- Data stored in: tickets.txt

## Controls
- Follow on-screen menu prompts
- Admin can manage all slots
- Users can only manage their own bookings
