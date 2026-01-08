package com.example.todoapi.service;

import com.example.todoapi.model.Task;
import com.example.todoapi.repository.TaskRepository;
import com.example.todoapi.dto.TaskRequest;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import java.util.Arrays;
import java.util.List;
import java.util.Optional;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.*;

@ExtendWith(MockitoExtension.class)
public class TaskServiceTest {
    
    @Mock
    private TaskRepository taskRepository;
    
    @InjectMocks
    private TaskService taskService;
    
    @Test
    void getAllTasks_ShouldReturnAllTasks() {
        // Arrange
        Task task1 = new Task("Task 1", "Description 1", Task.TaskStatus.TODO);
        Task task2 = new Task("Task 2", "Description 2", Task.TaskStatus.IN_PROGRESS);
        List<Task> tasks = Arrays.asList(task1, task2);
        
        when(taskRepository.findAll()).thenReturn(tasks);
        
        // Act
        List<Task> result = taskService.getAllTasks();
        
        // Assert
        assertEquals(2, result.size());
        assertEquals("Task 1", result.get(0).getTitle());
        verify(taskRepository, times(1)).findAll();
    }
    
    @Test
    void getTaskById_ExistingId_ShouldReturnTask() {
        // Arrange
        Task task = new Task("Task 1", "Description 1", Task.TaskStatus.TODO);
        task.setId(1L);
        
        when(taskRepository.findById(1L)).thenReturn(Optional.of(task));
        
        // Act
        Optional<Task> result = taskService.getTaskById(1L);
        
        // Assert
        assertTrue(result.isPresent());
        assertEquals("Task 1", result.get().getTitle());
        verify(taskRepository, times(1)).findById(1L);
    }
    
    @Test
    void getTaskById_NonExistingId_ShouldReturnEmpty() {
        // Arrange
        when(taskRepository.findById(999L)).thenReturn(Optional.empty());
        
        // Act
        Optional<Task> result = taskService.getTaskById(999L);
        
        // Assert
        assertFalse(result.isPresent());
        verify(taskRepository, times(1)).findById(999L);
    }
    
    @Test
    void createTask_ShouldSaveAndReturnTask() {
        // Arrange
        TaskRequest request = new TaskRequest("New Task", "New Description", Task.TaskStatus.TODO);
        Task savedTask = new Task("New Task", "New Description", Task.TaskStatus.TODO);
        savedTask.setId(1L);
        
        when(taskRepository.save(any(Task.class))).thenReturn(savedTask);
        
        // Act
        Task result = taskService.createTask(request);
        
        // Assert
        assertNotNull(result);
        assertEquals("New Task", result.getTitle());
        assertEquals(Task.TaskStatus.TODO, result.getStatus());
        verify(taskRepository, times(1)).save(any(Task.class));
    }
    
    @Test
    void deleteTask_ExistingId_ShouldDelete() {
        // Arrange
        when(taskRepository.existsById(1L)).thenReturn(true);
        
        // Act
        taskService.deleteTask(1L);
        
        // Assert
        verify(taskRepository, times(1)).existsById(1L);
        verify(taskRepository, times(1)).deleteById(1L);
    }
    
    @Test
    void deleteTask_NonExistingId_ShouldThrowException() {
        // Arrange
        when(taskRepository.existsById(999L)).thenReturn(false);
        
        // Act & Assert
        RuntimeException exception = assertThrows(RuntimeException.class, 
            () -> taskService.deleteTask(999L));
        
        assertEquals("Task not found with id: 999", exception.getMessage());
        verify(taskRepository, times(1)).existsById(999L);
        verify(taskRepository, never()).deleteById(any());
    }
}