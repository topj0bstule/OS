package com.example.todoapi;

import com.example.todoapi.model.Task;
import com.example.todoapi.repository.TaskRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.stereotype.Component;

@Component
public class DataInitializer implements CommandLineRunner {
    
    @Autowired
    private TaskRepository taskRepository;
    
    @Override
    public void run(String... args) throws Exception {
        // Создаем тестовые задачи, если база пуста
        if (taskRepository.count() == 0) {
            taskRepository.save(new Task(
                "Изучить Spring Boot", 
                "Пройти курс по Spring Boot и REST API", 
                Task.TaskStatus.IN_PROGRESS
            ));
            
            taskRepository.save(new Task(
                "Создать To-Do приложение", 
                "Разработать полное CRUD приложение с фронтендом", 
                Task.TaskStatus.TODO
            ));
            
            taskRepository.save(new Task(
                "Добавить XML валидацию", 
                "Реализовать валидацию через XSD схему", 
                Task.TaskStatus.DONE
            ));
        }
    }
}