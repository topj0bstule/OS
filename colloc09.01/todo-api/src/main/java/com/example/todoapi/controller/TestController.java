package com.example.todoapi.controller;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.ResponseBody;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

@Controller
public class TestController {
    
    @GetMapping("/tests")
    public String showTestPage() {
        return "test-runner"; // Ищет templates/test-runner.html
    }
    
    @PostMapping("/api/tests/run")
    @ResponseBody
    public TestResult runTests() {
        TestResult result = new TestResult();
        List<String> testOutput = new ArrayList<>();
        
        try {
            // Определяем команду для Windows/Unix
            String mavenCommand = System.getProperty("os.name").toLowerCase().contains("win") 
                ? "mvn.cmd" : "mvn";
            
            // Запускаем Maven тесты
            ProcessBuilder processBuilder = new ProcessBuilder();
            processBuilder.command(mavenCommand, "test", "-DskipTests=false");
            processBuilder.redirectErrorStream(true);
            
            // Указываем рабочую директорию (корень проекта)
            processBuilder.directory(new File(System.getProperty("user.dir")));
            
            Process process = processBuilder.start();
            
            // Читаем вывод
            BufferedReader reader = new BufferedReader(
                new InputStreamReader(process.getInputStream()));
            
            String line;
            while ((line = reader.readLine()) != null) {
                testOutput.add(line);
            }
            
            int exitCode = process.waitFor();
            
            result.setSuccess(exitCode == 0);
            result.setOutput(testOutput);
            result.setExitCode(exitCode);
            
        } catch (Exception e) {
            result.setSuccess(false);
            testOutput.add("Error running tests: " + e.getMessage());
            testOutput.add("Working dir: " + System.getProperty("user.dir"));
            result.setOutput(testOutput);
        }
        
        return result;
    }
    
    // Класс для результата тестов
    public static class TestResult {
        private boolean success;
        private List<String> output;
        private int exitCode;
        
        // Геттеры и сеттеры
        public boolean isSuccess() { return success; }
        public void setSuccess(boolean success) { this.success = success; }
        
        public List<String> getOutput() { return output; }
        public void setOutput(List<String> output) { this.output = output; }
        
        public int getExitCode() { return exitCode; }
        public void setExitCode(int exitCode) { this.exitCode = exitCode; }
    }
}