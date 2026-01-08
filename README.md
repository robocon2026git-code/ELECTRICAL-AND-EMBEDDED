\# GITHUB WORKFLOW GUIDE



Welcome to \*\*ROBOCON WORKSPACE\*\*

This document explains \*\*how team members should clone the repo, set up Git, and work using feature branches\*\*.



⚠️ Direct changes to `main` are NOT allowed.  

All work must be done using \*\*feature branches\*\* and merged via \*\*Pull Requests (PRs)\*\*.


---


\## 🧠 Git Basics (Quick Understanding)



\- \*\*Repository (repo)\*\* → Project stored with version control

\- \*\*main branch\*\* → Stable, reviewed code only

\- \*\*feature branch\*\* → Your personal workspace

\- \*\*commit\*\* → A saved snapshot of changes

\- \*\*origin\*\* → Remote GitHub repository

\- \*\*Pull Request (PR)\*\* → Request to merge code into `main`



---



\## One-Time Setup (Required on Every New PC)



Run these commands \*\*once\*\* after installing Git:



```bash

git config --global user.name "Your Name"

git config --global user.email "your-email@example.com"

git config --global init.defaultBranch main



\## To verify configuration

git config --global --list




**## Cloning and work steps starts from here:**



!!!!IMPORTANT ONE TIME SETTING AFTER INSTALLED GIT:

git clone https://github.com/robocon2026git-code/ELECTRO-CORE.git




# \## REGULAR WORK PROCEDURE

1)cd ELECTRO-CORE


2)git status


3)git checkout main					//This line says go to main branch


4)git pull origin main					//Important step otherwise you might miss some files that was updated recently


5)git checkout -b feature/short-description		//Creating your local workspace

    ex:

        git checkout -b feature/pneumatic\_actuation

	git checkout -b feature/arduino-motor

	git checkout -b feature/i2c-driver



After work done run this command once

6)git status



\## THE FOLLOWING STEPS ARE AFTER COMPLETION OF YOUR WORK

\*\* THIS THREE COMMENTS WE WILL USE FREQUENTLY

7)git add .


8)git commit -m "Added new functionality to drive train"


Run this command only when your first push after creation of your feature branch

9) a_)git push -u origin feature/your-branch-name


from next push onwards you can just type

   b_)git push



For local space

10) a_)git branch -d feature/your-branch-name		//Run this comment only after code is merged. This will delete your branch



For remote space

    b_)git push origin --delete feature/your-branch-name

